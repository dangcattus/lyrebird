/*File: fcfs.h
*Submitted by : Maxx Sokal  
*Submitted for: CMPT300 D1 Instructor: Brian Booth  TA: Scott Kristjanson
*/

#include "memwatch.h"

void fcfs(int socketNum);

void child(int* fdc, int* fdp);

void parent(int** fdc, int** fdp, int socketNum, unsigned int* pid);
