#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define SERVERPORT "4950"   // the port users will be connecting to
#define MAXBUFLEN 100
int main(int argc, char *argv[])
{
    int sockfd;
    socklen_t addr_len;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    if (argc != 3) {
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
    if ((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }
    printf("listener: waiting to recvfrom...\n");
    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    char output[MAXBUFLEN];
    int i = 0;
    int len = (int)strlen(buf);
    
    while(len >= 0){
	if(buf[len-1] >= '0' && buf[len-1] <= '9'){
		output[i] = buf[len-1];
		i++;
	}
	len --;
    }    
    output[i] = '\0';
    printf("OUTPUT : %s\n",output);
    freeaddrinfo(servinfo);
    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
    close(sockfd);
    return 0;
}
