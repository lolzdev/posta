#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include "utils.h"
#include "imap.h"

int main(void)
{
	SSL_library_init();

	imap_t *state = imap_connect("sagittarius-a.org", "993");

	/*
	if (imap_starttls(state) == 0) {
		printf("TLS connection enstablished!\n");
	} else {
		printf("Can't enstablish TLS connection\n");
		exit(-1);
	}*/

	if (imap_capability(state) != 0) {
		printf("Can't get server capabilities!\n");
		exit(-1);
	}

	if (state->capabilities & IMAP4) {
		printf("IMAP4rev1 found\n");
	}

	if (imap_login(state, "", "") != 0) {
		printf("Login failed!\n");
	} else {
		printf("Logged successfully!\n");
	}

	imap_close(state);

	return 0;
}
