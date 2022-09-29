#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>


#define PORT 6000

/*void *recver(void *arg) {

}

void *sender(voud *arg) {

}*/

// returns inet_pton value
int init_addr(int port, char *ip, struct sockaddr_in *addr) {
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	if (ip == NULL) {
		(addr->sin_addr).s_addr = htonl(INADDR_ANY);
		return 1;
	}
	return inet_pton(AF_INET, ip, &( (*addr).sin_addr.s_addr ));
}


// returns inet_pton value
int init_addr6(int port, char *ip, struct sockaddr_in6 *addr) {
	memset(addr, 0, sizeof(struct sockaddr_in6));
	addr->sin6_family = AF_INET6;
	addr->sin6_port = htons(port);
	if (ip == NULL) {
		addr->sin6_addr = in6addr_any;
		return 1;
	}
	return inet_pton(AF_INET6, ip, &(addr->sin6_addr));
}

void print_mem(void *buff, size_t size) {
	unsigned char byte;
	for (int i = 0; i < size; i++) {
		byte = ((char*)buff)[i];
		printf("%d ", byte);
	}
	printf("\n");
}

int main(int argc, char *argv[]) {

	// char iploopback[] = "127.0.0.1";


	char *group_ip = NULL;
	int domain = AF_INET;
	int opt = 0;

	while ( (opt = getopt(argc, argv, "v:g:")) != -1 ) {
		switch (opt) {
			case 'v':
				printf("optarg: %s\n", optarg);
				domain = atoi(optarg) == 4 ? AF_INET :
					 	 atoi(optarg) == 6 ? AF_INET6 :
						 atoi(optarg);
				break;
			case 'g':
				printf("optarg: %s\n", optarg);
				group_ip = optarg;
				break;
			default:
				printf("something wrong\nusage: [-v ip_version] [-g group_ip_addres]\n");
				return EXIT_FAILURE;
		}
	}

	printf("here\n");

	int sockfd = socket(domain, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		printf("socket error\n");
		return EXIT_FAILURE;
	}
	


	if (domain == AF_INET) printf("AF_INET");
	else if (domain == AF_INET6) printf("AF_INET6");

	

	/*

	wtf?????

	struct in_addr local_addr;
	local_addr.s_addr = htonl(INADDR_ANY);
	




	// sizeof(multiaddr) determines which structure was passed (struct ip_mreqn or struct ip_mreq (old version) or else)
	if ( setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &local_addr, sizeof(local_addr)) == -1) {
		perror("setsockopt error");
		return EXIT_FAILURE;
	}*/


	struct sockaddr_storage addr;
	memset(&addr, 0, sizeof(addr));
	int res;
	if (domain == AF_INET) {
		struct sockaddr_in addr4;
		memset(&addr4, 0, sizeof(addr4));
		res = init_addr(PORT, group_ip, &addr4);
		// addr = (struct sockaddr_storage)addr4;
		memcpy(&addr, &addr4, sizeof(addr4));
	}
	else if (domain == AF_INET6) {
		struct sockaddr_in6 addr6;
		memset(&addr6, 0, sizeof(addr6));
		res = init_addr6(PORT, group_ip, &addr6);
		// addr = (struct sockaddr_storage)addr6;
		memcpy(&addr, &addr6, sizeof(addr6));
	}
	if (res < 1) {
		if (res == 0) {
			printf("wrong ip form\n");
		}
		else {
			perror("inet_pton");
		}
		return EXIT_FAILURE;
	}
	/*struct sockaddr_in addr4;
	memset(&addr4, 0, sizeof(addr4));
	init_addr(PORT, NULL, &addr4);*/

	print_mem(&addr, sizeof(addr));

	int buff_leng = 50;
	char *buff = (char*)calloc(buff_leng, sizeof(char));
	for (int i = 0; i < 50; i++) {
		buff[i] = 'a' + rand() % 20;
	}
	printf("send: ");
	for (int i = 0; i < buff_leng; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");


	while (true) {
		sendto(sockfd, buff, buff_leng, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_storage));
		printf("sent\n");
		sleep(1);
	}
	

	printf("sent\n");
	


	return 0;
}