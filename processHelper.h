/*
* File: processHelper.h
* Author: Maxx Sokal  
* Submitted for CMPT 300D1 Instructor: Brian Booth  TA: Scott Kristjanson
*
*Created: October 9, 2015, 6:53PM
*/

#include "memwatch.h"
#include "lyrebird.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef PROCESSHELPER_H
#define PROCESSHELPER_H

void getTimeString(char* timebuf);
int openFile(FILE* file, char* fileName);
int** createPipeArray(int numChild);
#endif /* PROCESSHELPER_H */
