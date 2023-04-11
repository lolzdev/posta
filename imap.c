#include "imap.h"
#include "utils.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <openssl/err.h>

imap_t *imap_connect(const char *addr, const char *port)
{
	int sock;
	char *res;
	FILE *sockfile;
	imap_t *state;
	struct addrinfo* address = NULL;

    getaddrinfo(addr, port, 0, &address);
	struct sockaddr_in *p = (struct sockaddr_in *)address->ai_addr;

	if ((sock = create_connection(p)) == -1)
		return NULL;

	if ((sockfile = fdopen(sock, "r")) == NULL)
		return NULL;

	state = (imap_t *) malloc(sizeof(imap_t));
	state->sock = sock;
	state->sockfile = sockfile;

	if (strcmp(port, "993") == 0) {
		state->tls = 1;
		imap_starttls(state);
	} else {
		state->tls = 0;
	}
	
	res = imap_response(state);

	if (res[2] != 'O' || res[3] != 'K')
		return NULL;

	free(res);

	return state;
}

void imap_close(imap_t *state)
{
	close_connection(state->sock);
	fclose(state->sockfile);

	free(state);
}

char *imap_response(imap_t *state)
{
	size_t len;
	char *res;
	char c;
	int i;

	len = 15;
	res = (char *) calloc(len, sizeof(char));
 	
	i = 0;

	if (!state->tls) {
		c = fgetc(state->sockfile);
	} else {
		SSL_read(state->ssl, &c, 1);	
	}

	while (c != '\n') {
		res[i] = c;
		i++;

		if (i > len) {
			len = i+5;
			res = (char *) reallocarray(res, len, sizeof(char));
		}
		if (!state->tls) {
			c = fgetc(state->sockfile);
		} else {
			SSL_read(state->ssl, &c, 1);	
		}
	}

	if (i < len)
		res = (char *) reallocarray(res, i+1, sizeof(char));

	res[i] = '\0';

	return res;
}

int imap_send(imap_t *state, char *cmd)
{
	if (!state->tls) {
		return send(state->sock, (const void *)cmd, strlen(cmd), 0);
	} else {
		return SSL_write(state->ssl, (const void *)cmd, strlen(cmd)) > 0;
	}
}

int imap_capability(imap_t *state)
{
	state->capabilities = 0;
	char *cmd, *res, *token;

	cmd = "A0 CAPABILITY\n";
	imap_send(state, cmd);	
	res = imap_response(state);
	token = get_token(res);

	if (!imap_match_token(token, "*"))
		return -1;	

	token = get_token(NULL);

	if (!imap_match_token(token, TOK_CAPABILITY))
		return -1;	

	while ((token = get_token(NULL)) != NULL) {
		if (imap_match_token(token, TOK_IMAP4))
			state->capabilities |= IMAP4;
		else if (imap_match_token(token, TOK_STARTTLS))
			state->capabilities |= STARTTLS;
	}

	free(res);
	res = imap_response(state);
	free(res);

	return 0;
}

int imap_match_token(char *t1, const char *t2)
{
	return strcmp(t1, t2) == 0;
}

int imap_starttls(imap_t* state)
{
	if (!state->tls) {
		char *cmd, *res, *token;

		cmd = "A1 STARTTLS\n";
		imap_send(state, cmd);	
		res = imap_response(state);
		token = get_token(res);

		if (!imap_match_token(token, "A1"))
			return 1;	

		token = get_token(NULL);

		if (!imap_match_token(token, "OK"))
			return 2;

	}

	state->ssl_ctx = SSL_CTX_new(TLSv1_2_client_method());
	state->ssl = SSL_new(state->ssl_ctx);
	SSL_set_fd(state->ssl, state->sock);
	if (SSL_connect(state->ssl) == 0) {
		return -1;
	}

	state->tls = 1;

	return 0;
}

int imap_login(imap_t* state, char *username, char *password)
{
	char *cmd, *res, *token;

	size_t datalen = strlen(username) + strlen(password) + 12;
	cmd = calloc(datalen, sizeof(char));
	snprintf(cmd, datalen, "A2 LOGIN %s %s\n", username, password);
	imap_send(state, cmd);	
	res = imap_response(state);
	token = get_token(res);

	if (!imap_match_token(token, "*"))
		return -1;	

	token = get_token(NULL);

	if (!imap_match_token(token, TOK_CAPABILITY))
		return -1;	

	while ((token = get_token(NULL)) != NULL) {
		if (imap_match_token(token, TOK_IMAP4))
			state->capabilities |= IMAP4;
		else if (imap_match_token(token, TOK_STARTTLS))
			state->capabilities |= STARTTLS;
	}

	free(res);


	res = imap_response(state);
	token = get_token(res);

	if (!imap_match_token(token, "A2"))
		return 1;	

	token = get_token(NULL);
	
	if (!imap_match_token(token, TOK_OK))
		return 1;	

	return 0;
}
