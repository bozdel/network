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

#include "misc.h"

typedef struct {
	int domain;
	char *group_ip;
} args_t;




void *recver(void *arg) {

	args_t args = *((args_t*)arg);
	int domain = args.domain;
	char *group_ip = args.group_ip;

	int sockfd = socket(domain, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket error");
		// return EXIT_FAILURE;
	}


	// more then one receiver can receive the message

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		perror("setsockopt: SO_REUSEADDR");
		// return EXIT_FAILURE;
	}


	struct sockaddr_storage addr;
	int res = init_addr(domain, PORT, NULL, &addr);
	if (res < 1) {
		if (res == 0)
			fprintf(stderr, "wrong ip form\n");
		else
			perror("inet_pton");
		// return EXIT_FAILURE;
	}
	

	if ( bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_storage)) == -1 ) {
		perror("bind error");
	}
	
	
	// joining a multicast group

	join_group(domain, sockfd, group_ip);



	int buff_leng = 50;
	char *buff = (char*)calloc(buff_leng, sizeof(char));

	struct sockaddr_storage recv_addr;
	socklen_t addr_leng = sizeof(recv_addr);


	// add list to contain senders

	while (true) {
		if ( recvfrom(sockfd, buff, buff_leng, 0, (struct sockaddr*)&recv_addr, &addr_leng) == -1 ) {
			perror("recvfrom error");
			// return EXIT_FAILURE;
		}
		printf("received\n");
		print_addr((struct sockaddr*)&recv_addr);

		for (int i = 0; i < buff_leng; i++)
			printf("%c", buff[i]);
		printf("\n");
	}
}

void *sender(void *arg) {

	args_t args = *((args_t*)arg);
	int domain = args.domain;
	char *group_ip = args.group_ip;

	int sockfd = socket(domain, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		printf("socket error\n");
		// return EXIT_FAILURE;
	}
	

	/*struct in_addr local_addr;
	local_addr.s_addr = htonl(INADDR_ANY);
	// sizeof(multiaddr) determines which structure was passed (struct ip_mreqn or struct ip_mreq (old version) or else)
	if ( setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &local_addr, sizeof(local_addr)) == -1 ) {
		perror("setsockopt error");
		// return EXIT_FAILURE;
	}*/

	/*int optval = 0;
	if ( setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &optval, sizeof(int)) == -1 ) {
		perror("setsockopt error IP_MULTICAST_LOOP");
		// return EXIT_FAILURE;
	}*/

	struct sockaddr_storage addr;
	int res = init_addr(domain, PORT, NULL, &addr);
	if (res < 1) {
		if (res == 0)
			fprintf(stderr, "wrong ip form\n");
		else
			perror("inet_pton");
		// return EXIT_FAILURE;
	}


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
		// printf("sent\n");
		sleep(1);
	}
}







int main(int argc, char *argv[]) {

	args_t args;
	/*char *group_ip = NULL;
	int domain = AF_INET;*/
	int opt = 0;

	while ( (opt = getopt(argc, argv, "v:g:")) != -1 ) {
		switch (opt) {
			case 'v':
				// version = atoi(optarg);
				args.domain = atoi(optarg) == 4 ? AF_INET :
					 	 atoi(optarg) == 6 ? AF_INET6 :
						 atoi(optarg);
				break;
			case 'g':
				args.group_ip = optarg;
				break;
			default:
				printf("something wrong\nusage: [-v ip_version] [-g group_ip_addres]\n");
				return EXIT_FAILURE;
		}
	}

	pthread_t recv, send;
	pthread_create(&send, NULL, sender, (void *)&args);
	pthread_create(&recv, NULL, recver, (void *)&args);

	sleep(100);

	pthread_join(send, NULL);
	pthread_join(recv, NULL);


	return 0;
}