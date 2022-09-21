#ifndef MISC_H
#define MISC_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6000

// returns inet_pton value
int init_addr4(int port, char *ip, struct sockaddr_in *addr);


// returns inet_pton value
int init_addr6(int port, char *ip, struct sockaddr_in6 *addr);

// returns inet_pton value
int init_addr(int domain, int port, char *ip, struct sockaddr_storage *addr);

int join_group(int domain, int sockfd, char *group_ip);

void print_addr(struct sockaddr *addr);

#endif