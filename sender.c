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

#define IPv4 0
#define IPv6 1

#define PORT 6000

/*void *recver(void *arg) {

}

void *sender(voud *arg) {

}*/

struct sockaddr_in init_addr(int port, char *ip) {
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (ip == NULL) {
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		printf("loopback addres is set\n");
		return addr;
	}
	int res = inet_pton(AF_INET, ip, &(addr.sin_addr));
	if (res == 0) {
		printf("wrong ip\n");
	}
	else if (res == -1) {
		printf("iner_pton error\n");
	}
	else {
		printf("inet_pton - ok\n");
	}
	return addr;
}



int main(int argc, char *argv[]) {

	// char iploopback[] = "127.0.0.1";


	char *group_ip = NULL;
	int version = IPv4;
	int opt = 0;

	while ( (opt = getopt(argc, argv, "vg:")) != -1 ) {
		switch (opt) {
			case 'v':
				version = atoi(optarg);
				break;
			case 'g':
				group_ip = optarg;
				break;
			default:
				printf("something wrong\nusage: [-v ip_version] [-g group_ip_addres]\n");
				return EXIT_FAILURE;
		}
	}


	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		printf("socket error\n");
		return EXIT_FAILURE;
	}
	




	

	/*

	wtf?????

	struct in_addr local_addr;
	local_addr.s_addr = htonl(INADDR_ANY);
	




	// sizeof(multiaddr) determines which structure was passed (struct ip_mreqn or struct ip_mreq (old version) or else)
	if ( setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &local_addr, sizeof(local_addr)) == -1) {
		perror("setsockopt error");
		return EXIT_FAILURE;
	}*/




	struct sockaddr_in addr = init_addr(PORT, group_ip);



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
		sendto(sockfd, buff, buff_leng, 0, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
		printf("sent\n");
		sleep(1);
	}
	

	printf("sent\n");
	


	return 0;
}