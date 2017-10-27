/*File: socketHelper.c
* Submitted by: Maxx Sokal,
*Submitted for: CMPT300 D1, Instructor: Brian Booth, TA: Scott Kristjanson
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "socketHelper.h"
#include "memwatch.h"

#define Debug 0
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void getClientIP(char* clientIP, struct sockaddr_in client,struct sockaddr_storage client_addr){
	char temp[INET6_ADDRSTRLEN];
	/*do the inet_ntop, how to send struct*/
		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr),temp, sizeof(temp));
		strcpy(clientIP, temp);
		return;

}

void setMemString(char* timebuf, char* clientIP, FILE* f){
	timebuf = malloc(26*sizeof(char) + 1);
	if (timebuf == NULL){
		fprintf(f, "Error allocating memory. Exiting.\n");
		exit(1);
	}
	else{
		clientIP = malloc(INET6_ADDRSTRLEN* sizeof(char)+2);
		if ( clientIP == NULL){
			fprintf(f, "Error allocating memory. Exiting.\n");
			exit(1);
		}
	}
	return;
}

char* getIPAddr(){
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs* ifa = NULL;
	void * tmpAddrPtr = NULL;
    char* myIP = (char*)malloc(INET_ADDRSTRLEN + 1);
	getifaddrs(&ifAddrStruct);

	for(ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next){
		if (!ifa->ifa_addr){
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET){
		/* check if it is valid IP4 address*/
			tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmpAddrPtr, myIP, INET_ADDRSTRLEN);
			#if (Debug)
				printf("%s IP Address %s\n", ifa->ifa_name, myIP);
			#endif
		}
		else if(ifa->ifa_addr->sa_family == AF_INET6){
			/*check if it is valid IP6 address*/
			tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			#if (Debug)
				printf("%s IP Address %s \n", ifa->ifa_name, addressBuffer);
			#endif
		}

	}
	if(ifAddrStruct!= NULL){
		 freeifaddrs(ifAddrStruct);
	}
	return myIP;
}
