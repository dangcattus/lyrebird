/*File: main.c
* Author: Maxx Sokal 
* Submitted for: CMPT 300D1  Instructor: Brian Booth  TA: Scott Kristjanson
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#include "memwatch.h"
#include "socketHelper.h"
#include "processHelper.h"

#define Debug 0
#define PORT 4444 /*define the port users will connect to*/
#define BACKLOG 10 /*define how manypending connections qeueue will hold*/
#define MAXSTRLEN 2050

int main (int argc, char* argv[]){

	FILE *config;
	FILE *logfile;
	char* timebuf = malloc(26*sizeof(char)+1);
	char* clientIP = malloc(INET6_ADDRSTRLEN * sizeof(char) + 2);
	int ALLDONE = 0;
	int openConnections = 0;
	config = fopen(argv[1], "r");
	int isOpen = openFile(config,argv[1]);
	if (!isOpen){
		getTimeString(timebuf);
		printf("%s File %s could not open. Exiting.\n",timebuf, argv[1]);
		exit(1);
	}
	logfile = fopen(argv[2], "w+");
	isOpen = openFile(logfile, argv[2]);
	if (!isOpen){
		getTimeString(timebuf);
		printf("%s File %s could not open. Exiting.\n", timebuf,argv[2]);
		exit(1);
	}
	int fdmax;
	int nbytes;
	int new_fd; /*newly accepted client fd*/
	struct sockaddr_storage client_addr;/*client address*/
	socklen_t sin_size;/*size of client address (?)*/
	char buf[2049];/*buffer for client data*/

	int listenfd = 0;
	struct sockaddr_in serv_addr;
	char ready[] = "READY";
	char gameover[] = "GAMEOVER";
	char* myIP = NULL;	/*stores server's IP address*/

/*variables to print client IP addresses*/
	struct sockaddr_in client;
/*	socklen_t client_size;*/

	myIP = getIPAddr();
	char line[2049];
	int i;
	int yes = 1;
	fd_set master, read_fds;
	getTimeString(timebuf);
	fprintf(logfile,"%s lyrebird.server: pid %d on host %s, port %u\n",timebuf, getpid(), myIP, PORT);
	printf("%s lyrebird.server: pid %d on host %s, port %u\n",timebuf,  getpid(), myIP, PORT);
	/*fill out server info*/
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);/*not in Beej tut*/

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(buf,'0', sizeof(buf));/*check if this is necessary*/
	serv_addr.sin_family = AF_UNSPEC;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	#if (Debug)
		printf("Server waiting for connection...\n");
	#endif
	if (listen(listenfd, 10) == -1){
		getTimeString(timebuf);
		printf("%s Error with listen. Exiting.\n", timebuf);
		/*function to free*/
		exit(3);
	}
	int whilecont = 1;
	FD_SET(listenfd, &master); /*add the listener to the master set*/
	fdmax = listenfd; /*keep track of all the fds*/
	while(whilecont){ /*main loop*/
		read_fds = master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1){
			getTimeString(timebuf);
			fprintf(logfile,"%s Lyrebird server: error with select function. Exiting.\n", timebuf);
			exit(4);
		}
	/*look through exisiting connections*/
		for ( i = 0; i<= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				#if (Debug)
					printf("FD ISSET! FD MAX IS %d\n", fdmax);
				#endif

				if (i == listenfd){ /*new connection on the listener fd!*/
					sin_size = sizeof(client_addr);
					new_fd = accept(listenfd, (struct sockaddr *)&client_addr, &sin_size);
					#if (Debug)
						printf("NEW FD on lyrebird server\n");
					#endif
					if(new_fd == -1){
						perror("accept");
					/*	continue;*/
					}
					else{
						FD_SET(new_fd, &master); /*add new client to the master set*/
						if (new_fd > fdmax){
							fdmax = new_fd;
						}
						getTimeString(timebuf);
						getClientIP(clientIP, client, client_addr);
						fprintf(logfile,"%sSuccessfully connected to lyrebird client %s.\n",timebuf, clientIP);
						openConnections ++;
					}
				}
				else{
					if((nbytes = recv(i, buf, sizeof(buf), 0)) <=0){
						if (nbytes == 0){
							getTimeString(timebuf);
							getClientIP(clientIP, client, client_addr);
							fprintf(logfile,"%s The lyrebird client %s has disconnected unexpectedly.\n", timebuf, clientIP);

						}
						else{
							getTimeString(timebuf);
							getClientIP(clientIP, client, client_addr);
							fprintf(logfile,"%s The lyrebird client %s has disconnected unexpectedly.\n", timebuf, clientIP);

						}
						close(i);
						FD_CLR(i, &master);
					}
					else{
						buf[nbytes] = '\0';
						if (strcmp(ready, buf) == 0){
							if (fgets(line, sizeof(line), config) != NULL){
								char input[1025];
								sscanf(line, "%s", input);
								getClientIP(clientIP, client, client_addr);
								getTimeString(timebuf);
								fprintf(logfile,"%s The lyrebird client %s has been given the task of decrypting %s.\n",timebuf,clientIP, input);
								send(i, line, strlen(line),0);
								#if (Debug)
									sleep(1);
								#endif

							}
							else{
								#if (Debug)
									printf("Nothing to decrypt.\n");
								#endif
								send (i, gameover, strlen(gameover), 0);
								/*	whilecont = 0;*/
								ALLDONE = 1;
							}
						}
						else{
							char errStr[MAXSTRLEN];
							char inStr[MAXSTRLEN];
							sscanf(buf, "%s%s",errStr, inStr);
							if (strcmp(errStr, "COMPLETE") == 0){
								getClientIP(clientIP, client, client_addr);
								getTimeString(timebuf);
								fprintf(logfile,"%s The lyrebird client %s has successfully decrypted %s\n",timebuf,clientIP, inStr);
							/*should contain child's pid Number that disconnected*/
							}
							else if(strcmp(errStr, "FILE") == 0){
								getClientIP(clientIP, client, client_addr);
								getTimeString(timebuf);
								fprintf(logfile,"%sThe lyrebird client %s has encountered an error: Unable to open file %s. \n",timebuf,clientIP, inStr);
							}
							else if(strcmp(errStr, "EXITSUCCESS") == 0){
								getClientIP(clientIP, client, client_addr);
								getTimeString(timebuf);
								fprintf(logfile, "%sThe lyrebird client %s has exited successfully.\n",timebuf, clientIP);
								close(i);
								FD_CLR(i, &master);
								openConnections--;
								if((openConnections == 0) && (ALLDONE == 1)){
									whilecont = 0;
								}

							}
						}
					}
				}
			}
		}
	}
	free(clientIP);
	free(myIP);
	free(timebuf);
	fclose(logfile);
	return 0;
}
