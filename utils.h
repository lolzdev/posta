#ifndef UTILS_H
#define UTILS_H

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>

int create_connection(struct sockaddr_in *addr);
void close_connection(int socket);

char *get_token(char *s);

#endif
