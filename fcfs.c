/*File: fcfs.c
*Submitted by : Maxx Sokal 
*Submitted for: CMPT300 D1 Instructor: Brian Booth  TA: Scott Kristjanson
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>

#include "fcfs.h"
#include "processHelper.h"
#include "lyrebird.h"
#include "memwatch.h"

#define Debug 0
#define READ 0
#define WRITE 1
#define MSGSIZE 2050
void fcfs(int socketNum){

	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	int numChild = numCPU-1;
	int **fdparent, **fdchild;
	int i;
	unsigned int *pidArray = NULL;
	pidArray = (unsigned int*)malloc(numChild * sizeof(unsigned int));
	if (pidArray == NULL){
		/*getTimeString();*/
		printf("Could not allocate memory in parent process. Exiting.\n");
		exit(1);
	}
	int parentpid;
	/*Allocate array for file descriptors*/
	fdparent = createPipeArray(numChild); /*for parent to write to child*/
	fdchild = createPipeArray(numChild); /*for child to write to parent*/

	/*create pipes*/
	for (i = 0; i<numChild; i++){
		if((pipe(fdparent[i]) == -1) || (pipe(fdchild[i]) == -1)){
			/*getTimeString();*/
			printf("Could not create pipes. Exiting\n");
			exit(1);
		}
	/*Fork children and assign them a pipe*/
		int pid = fork();
		if (pid < 0){
			/*getTimeString();*/
			printf("Fork failed. Exiting\n");
			exit(1);
		}
		else{
			if(pid == 0){
				child(fdchild[i], fdparent[i]);
				int k;
				for (k = 0; k< numChild; i++){
					free(fdparent[i]);
					free(fdchild[i]);
				}
				free(fdparent);
				free(fdchild);
				free(pidArray);
				exit(0);
			}
			else{
				pidArray[i] = pid;
				parentpid = getpid();
			}
		}
	}
	if(getpid() == parentpid){
		parent(fdchild, fdparent, socketNum, pidArray);
	}
/*free memory of file descriptor array*/
	for (i = 0; i < numChild; i++){
		free(fdchild[i]);
		free(fdparent[i]);
	}
	free(fdchild);
	free(fdparent);
	free(pidArray);
}

void child(int* fdc, int* fdp){
	char buf[MSGSIZE];
	char sendBuf[MSGSIZE];
	char input[1025];
	char output[1025];
	char ready[] = "READY";
	char gameover[] = "GAMEOVER";
	int keepGoing = 1;
	close(fdp[WRITE]);
	close(fdc[READ]);
	while (keepGoing){
		write(fdc[WRITE], ready, MSGSIZE);
		if (read(fdp[READ],buf, MSGSIZE)>0){
			if(strcmp(buf, gameover) == 0){
				keepGoing = 0;
			}
			else{
				sscanf(buf, "%s%s", input, output);
				#if (Debug)
					printf("Child #%i input: %s and output: %s\n",getpid(), input, output);
				#endif
				FILE* inFile = fopen(input, "r");
				int isOpen;
				isOpen = openFile(inFile, input);
				if (isOpen == 0){
					strcpy(sendBuf, "FILE ");
					strcat(sendBuf, input);
					write(fdc[WRITE], sendBuf, MSGSIZE);
				}
				else{
					FILE* outFile = fopen(output, "w");
					isOpen = openFile(outFile, output);
					if (isOpen == 0) {
						fclose(inFile);
						strcpy(sendBuf, "FILE ");
						strcat(sendBuf, output);
						write(fdc[WRITE],sendBuf, MSGSIZE);
					}
					else{
						decrypt(inFile, outFile);
						strcpy(sendBuf, "COMPLETE ");
						strcat(sendBuf, input);
						write(fdc[WRITE], sendBuf, MSGSIZE);
						fclose(inFile);
						fclose(outFile);
					}
				}
			/*	write(fdc[WRITE],ready,MSGSIZE);*/
			}
		}
	}/*end while keepGoing*/
	close(fdc[WRITE]);
	close(fdc[READ]);
	return;
}

void parent(int** fdc, int** fdp, int socketNum, unsigned int* pidArray){
	#if (Debug)
		printf("Enter Parent in FCFS! SOcket number is %i\n", socketNum);
	#endif
	char line[2049], buf[2049];
	fd_set fdread, fdread_copy,fdwrite;
	int numChild = sysconf(_SC_NPROCESSORS_ONLN)-1;
	int i, bytes_rec;
	char* timebuf = malloc(26 * sizeof(char) + 1);
	if (timebuf == NULL){
		printf("lyrebird.client: error with malloc. Exiting\n");
		exit(1);
	}
	char ready[] = "READY";
	char gameover[] = "GAMEOVER";
	char exitsuccess[] = "EXITSUCCESS";
	/*create descriptor mask*/
	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	/*add read and write end of each pipe to parent's fdset*/
	for(i = 0; i< numChild; i++){
		FD_SET(fdp[i][WRITE], &fdwrite);
		FD_SET(fdc[i][READ], &fdread);
	}
	for (i = 0; i < numChild; i++){
/*	close reading and writing  for parent processes to pass txt to children*/
		close(fdp[i][READ]);
		close(fdc[i][WRITE]);
	}
	fdread_copy = fdread;
	while (select(fdc[numChild - 1][READ]+1,&fdread_copy, NULL, NULL, NULL) > 0){
		for(i = 0; i< numChild; i++){
			if (FD_ISSET(fdc[i][READ],&fdread_copy)){
				if (read(fdc[i][READ], buf,MSGSIZE) > 0){
					if (strcmp(buf, ready) == 0){
						send(socketNum, ready, strlen(ready), 0);
						if((bytes_rec = recv(socketNum, line, sizeof(line), 0)) >=0 ){
							if( bytes_rec == 0){
								getTimeString(timebuf);
								printf("%slyrebird.client: Server closed connection. Exiting.\n", timebuf);
								break; /*check here to see if memwatch no anomolies*/
							}
							else{
								line[bytes_rec] = '\0';
								#if (Debug)
									printf("CLIENT received: LINE: %s\n", line);
								#endif
								char inString[1025];
								sscanf(line, "%s", inString);
								write(fdp[i][WRITE], line, MSGSIZE);
							}
						}
						else{
							#if (Debug)
								printf("PARENT:: NO MORE FILES. CLOSING\n");
							#endif
							write(fdp[i][WRITE],gameover,MSGSIZE);
						}
					}
					else{
						/*received message from child to send from socket*/
						char errMsg[MSGSIZE];
						char inStr[MSGSIZE];
						sscanf(buf, "%s%s", errMsg, inStr);
						send(socketNum, buf, sizeof(buf), 0);
					}
				}
			}
		}
		if (waitpid(-1,NULL,WNOHANG) == -1){
			#if (Debug)
				printf("Completed decryption of all files. Parent ID# %d exiting.\n",getpid());
			#endif
			send(socketNum, exitsuccess, strlen(exitsuccess),0);
			free(timebuf); /*this just added*/
			return;
		}
		fdread_copy = fdread;
	}
	free(timebuf);
}
