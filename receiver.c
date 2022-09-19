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

// fix return value
int join_group(int sockfd, char *group_ip, int domain) {
	if (domain == AF_INET6) {
		struct ipv6_mreq multiaddr;
		switch (inet_pton(AF_INET6, group_ip, &multiaddr.ipv6mr_multiaddr)) {
			case -1:
				perror("inet_pton error");
				return EXIT_FAILURE
			case 0:
				printf("invalid address\n");
				break;
			default:
				printf("inet_pton - ok\n");
				break;
		}
		multiaddr.ipv6mr_ifindex = 0;

		// sizeof(multiaddr) determines which structure was passed (struct ip_mreqn or struct ip_mreq (old version))
		if ( setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multiaddr, sizeof(multiaddr)) == -1) {
			perror("setsockopt error");
			return EXIT_FAILURE;
		}
		return 10; //???
	}
	
	struct ip_mreqn multiaddr;

	// make it in network byte-order
	switch (inet_pton(AF_INET, group_ip, &multiaddr.imr_multiaddr)) {
		case -1:
			perror("inet_pton error");
			return EXIT_FAILURE;
		case 0:
			printf("invalid address\n");
			break;
		default:
			printf("inet_pton - ok\n");
			break;
	}
	multiaddr.imr_address.s_addr = htonl(INADDR_ANY); // make it in network byte-order
	multiaddr.imr_ifindex = 0; // any interface (what is it?)



	// sizeof(multiaddr) determines which structure was passed (struct ip_mreqn or struct ip_mreq (old version))
	if ( setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multiaddr, sizeof(multiaddr)) == -1) {
		perror("setsockopt error");
		return EXIT_FAILURE;
	}
}

void print_addr(struct sockaddr *addr) {
	char ip[INET_ADDRSTRLEN] = { 0 };
	char ip6[INET6_ADDRSTRLEN] = { 0 };
	switch (addr->sa_family) {
		case AF_INET:
			if (inet_ntop(AF_INET, (struct sockaddr_in*)&addr, ip, INET_ADDRSTRLEN) == NULL) {
				perror("inet_ntop error");
				return;
			}
			printf("ip: %s\n", ip);
			printf("port: %d\n", ntohs( ((struct sockaddr_in*)addr)->sin_port));
			break;
		case AF_INET6:
			if (inet_ntop(AF_INET6, (struct sockaddr_in6*)&addr, ip6, INET6_ADDRSTRLEN) == NULL) {
				perror("inet_ntop error");
				return;
			}
			printf("ip: %s\n", ip6);
			printf("port: %d\n", ntohs( ((struct sockaddr_in6*)addr)->sin6_port));
			break;
		default:
			printf("incompatable address family\n");
			break;
	}
}



int main(int argc, char *argv[]) {

	char *group_ip = NULL;
	int version = IPv4;
	int domain = AF_INET;
	int opt = 0;

	while ( (opt = getopt(argc, argv, "vg:")) != -1 ) {
		switch (opt) {
			case 'v':
				// version = atoi(optarg);
				domain = atoi(optarg) == 4 ? AF_INET :
					 	 atoi(optarg) == 6 ? AF_INET6 :
						 atoi(optarg);
				break;
			case 'g':
				group_ip = optarg;
				break;
			default:
				printf("something wrong\nusage: [-v ip_version] [-g group_ip_addres]\n");
				return EXIT_FAILURE;
		}
	}



	int sockfd = socket(domain, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket error");
		return EXIT_FAILURE;
	}


	// more then one receiver can receive the message

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		perror("setsockopt: SO_REUSEADDR");
		return EXIT_FAILURE;
	}


	struct sockaddr_storage addr;
	int res;
	if (domain == AF_INET) {
		struct sockaddr_in addr4;
		res = init_addr(PORT, NULL, &addr4);
		// addr = (struct sockaddr_storage)addr4;
		memcpy(&addr, &addr4, sizeof(addr4));
	}
	else if (domain == AF_INET6) {
		struct sockaddr_in6 addr6;
		res = init_addr6(PORT, NULL, &addr6);
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

	if ( bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1 ) {
		perror("bind error");
	}
	
	
	// joining a multicast group

	join_group(sockfd, group_ip);







	int buff_leng = 50;
	char *buff = (char*)calloc(buff_leng, sizeof(char));

	struct sockaddr_in recv_addr;
	socklen_t addr_leng = sizeof(recv_addr);


	// add list to contain senders

	while (true) {
		if ( recvfrom(sockfd, buff, buff_leng, 0, (struct sockaddr*)&recv_addr, &addr_leng) == -1 ) {
			perror("recvfrom error");
			return EXIT_FAILURE;
		}


		if (addr_leng > sizeof(recv_addr)) {
			printf("addr was trancated\n"
					"the buffer is to small\n");
		}
		else {
			print_addr((struct sockaddr*)&recv_addr);
		}


		for (int i = 0; i < buff_leng; i++) {
			printf("%c", buff[i]);
		}
		printf("\n");
	}
	



	


	return 0;
}