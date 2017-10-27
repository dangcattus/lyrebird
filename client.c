#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "socketHelper.h"
#include "fcfs.h"
#include "processHelper.h"
#include "memwatch.h"

#define MAXDATASIZE 2050
#define Debug 0

int main(int argc, char*argv[]){
	int sockfd = 0;
	char buf[MAXDATASIZE];
	unsigned int port;
	struct sockaddr_in serv_addr;
	char* timebuf = malloc(26*sizeof(char)+1);
	if (timebuf == NULL){
		printf("Could not allocate memory. Exiting.\n");
		exit(1);
	}
	if (argc != 3){
		getTimeString(timebuf);
		printf("%sNot enough arguments for lyrebird.client. Exiting. \n",timebuf);
		free(timebuf);
		return 1;
	}
	if (sscanf(argv[2], "%u", &port) != 1){
		getTimeString(timebuf);
		printf("%sPort scan failed. Exiting.\n", timebuf);
		free(timebuf);
		exit(1);
	}
	memset(buf, '0', sizeof(buf));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		getTimeString(timebuf);
		printf("%s lyrebird.client: error creating socket. Exiting.\n", timebuf);
		free(timebuf);
		return 1;
	}
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons((uint16_t)port);
	
	if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0){
		getTimeString(timebuf);
		printf("%s lyrebird.client: inet_pton error occured. Exiting\n", timebuf);
		free(timebuf);
		return 1;
	}
	
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		getTimeString(timebuf);
		printf("%slyrebird.client: Connect failed. Exiting.\n", timebuf);
		free(timebuf);
		return 1;
	}
	else{
		fcfs(sockfd);
		#if (Debug)
			sleep(4);
		#endif
	}
	free(timebuf);
	return 0;
}
