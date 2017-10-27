/*
* File: processHelper.c
* Author: Maxx Sokal  
* Submitted for: CMPT 300D1  Instructor: Brian Booth  TA: Scott Kristjanson
*
* Created on  Oct. 9 2015, 6:51PM
*/

#include "processHelper.h"
#include "memwatch.h"
#include "lyrebird.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void getTimeString(char* timebuf){
/*Returns the current time in a formatted string without the newLine*/
	time_t currentTime;
	time(&currentTime);
	char timeString[24];
	strncpy(timeString,ctime(&currentTime),strlen(ctime(&currentTime))-1);
	strcpy(timebuf, "[");
	strcat(timebuf,timeString);
	strcat(timebuf,"]");
}

int openFile(FILE* file, char* fileName){
/*Open a file and returns error message 0 for cannot open and 1 for open success*/

	if (file == NULL){

		return 0;
	}
	else{
		return 1;
	}
}

int** createPipeArray(int numChild){
	int** pipe;
	int i;
	pipe = malloc(numChild * sizeof(int*));
	if (pipe == NULL){
		/*getTimeString();*/
		printf("Could not allocate memory. Exiting.\n");
		exit(1);
	}
	for (i = 0; i< numChild; i++){
		pipe[i] = malloc(2*sizeof(int));
		if (pipe[i] == NULL){
			/*getTimeString();*/
			printf("Could not allocate memory. Exiting\n");
		}
	}
	return pipe;
}
