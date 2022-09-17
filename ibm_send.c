#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


struct in_addr        localInterface;
struct sockaddr_in    groupSock;
int                   sd;
int                   datalen;
char                  databuf[1024];

int main (int argc, char *argv[])
{

  /* ------------------------------------------------------------*/
  /*                                                             */
  /* Send Multicast Datagram code example.                       */
  /*                                                             */
  /* ------------------------------------------------------------*/

  /*
   * Create a datagram socket on which to send.
   */
  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sd < 0) {
    perror("opening datagram socket");
    exit(1);
  }

  /*
   * Initialize the group sockaddr structure with a
   * group address of 225.1.1.1 and port 5555.
   */
  memset((char *) &groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr("225.1.1.1");
  groupSock.sin_port = htons(8395);

  /*
   * Disable loopback so you do not receive your own datagrams.
   */
  {
    char loopch=0;

    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP,
                   (char *)&loopch, sizeof(loopch)) < 0) {
      perror("setting IP_MULTICAST_LOOP:");
      close(sd);
      exit(1);
    }
  }

  /*
   * Set local interface for outbound multicast datagrams.
   * The IP address specified must be associated with a local,
   * multicast-capable interface.
   */
  // "9.5.1.1"
  localInterface.s_addr = /*htonl("192.168.1.2")*/inet_addr("192.168.1.2");
  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF,
                 (char *)&localInterface,
                 sizeof(localInterface)) < 0) {
  	switch (errno) {
  		case EFAULT:
  			printf("efault\n");
  			break;
  		case EBADF:
  			printf("ebadf\n");
  			break;
  		case EINVAL:
  			printf("einvel\n");
  			break;
  		case ENOPROTOOPT:
  			printf("enopr\n");
  			break;
  		case ENOTSOCK:
  			printf("enotsock\n");
  			break;
  		case EADDRNOTAVAIL:
  			printf("df\n");
  			break;
  		default:
  			printf("something else\n");
  			break;
  	}
    perror("setting local interface");
    exit(1);
  }


  /*
   * Send a message to the multicast group specified by the
   * groupSock sockaddr structure.
   */
  datalen = 10;
  for (int i = 0; i < 30; i++) {
  	databuf[i] = 'a' + rand() % 10;
  }
  while (1) {
  	sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
  	printf("sent\n");
  	sleep(1);
  }
  /*if ( < 0)
  {
    perror("sending datagram message");
  }*/
}