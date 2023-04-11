#include "utils.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

int create_connection(struct sockaddr_in *endpoint)
{
	int fd, status;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	if ((status = connect(fd, (struct sockaddr*)endpoint, sizeof(*endpoint))) < 0) {
		printf("Failed to connect!\n");
		return -1;
	}

	return fd;
}

void close_connection(int socket)
{
	close(socket);
}

char *get_token(char *s)
{	
	const char sep[4] = " ";
	return strtok(s, sep);
}
