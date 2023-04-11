#ifndef IMAP_H
#define IMAP_H

#include <stdio.h>
#include <openssl/ssl.h>

#define TOK_OK "OK"
#define TOK_BAD "BAD"
#define TOK_CAPABILITY "CAPABILITY"
#define TOK_IMAP4 "IMAP4rev1"
#define TOK_STARTTLS "STARTTLS"

enum imap_caps {
	IMAP4 = 1 << 0,
	STARTTLS = 1 << 1,	
};

/* IMAP connection */
typedef struct {
	int sock;
	int capabilities;
	int tls;
	FILE *sockfile;
	SSL *ssl;
	SSL_CTX *ssl_ctx;
} imap_t;

/* Connect to an IMAP server */
imap_t *imap_connect(const char *addr, const char *port);

/* Close the connection to the IMAP server, automatically free used memory */
void imap_close(imap_t *state);

/* Get the server response as a string */
char *imap_response(imap_t *state);

int imap_send(imap_t *state, char *cmd);

/* Query for server capabilities */
int imap_capability(imap_t *state);

/* Check for token matching */
int imap_match_token(char *t1, const char *t2);

/* Init TLS connection */
int imap_starttls(imap_t* state);

/* Login to the IMAP server */
int imap_login(imap_t* state, char *username, char *password);

#endif
