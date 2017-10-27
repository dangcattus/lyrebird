/*
 * File:   lyrebird.h
 * Author: Maxx Sokal	
 * Submitted for: CMPT 300D1	Instructor:Brian Booth	TA:Scott Kristjanson
 *
 * Created on September 9, 2015, 10:20 PM
 */

#include "memwatch.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef LYREBIRD_H
#define	LYREBIRD_H

void decrypt(FILE* inputFile, FILE* outputFile);

char* removeEight(char* tweet, unsigned int* tweetLength);

unsigned int* base41(char* tweet,unsigned int* sumTweet, unsigned int* tweetLength);

unsigned int* cModn(unsigned int* sumTweet,unsigned int* modTweet, unsigned int* arrayLength);

unsigned long long int intPower(unsigned long long base, unsigned long long int exp);

unsigned long long int modExp(unsigned long long int a, unsigned long long int b);

unsigned long long int Mod(unsigned long long int a, unsigned long long int n);

void invBase41(unsigned int* modArrayLength, unsigned int* modTweet, FILE* out);

#endif	/* LYREBIRD_H */
