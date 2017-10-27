/*File: socketHelper.h
* Submitted by: Maxx Sokal
*Submitted for: CMPT300 D1, Instructor: Brian Booth, TA: Scott Kristjanson
*/

#ifndef SOCKETHELPER_H
#define SOCKETHELPER_H
#include "sys/socket.h"

#include "memwatch.h"

void *get_in_addr(struct sockaddr *sa);

void getClientIP(char* clientIP, struct sockaddr_in client, struct sockaddr_storage clientaddr);

void setMemString(char* timebuf, char* clientIP, FILE* f);
char* getIPAddr();
#endif /*SOCKETHELPER_H*/
