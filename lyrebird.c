/*
 * File:   lyrebird.c
 * Author: Maxx Sokal	
 * Submitted for: CMPT 300D1	Instructor:Brian Booth	TA:Scott Kristjanson
 *
 * Created on September 9, 2015, 10:22 PM
 */

#include "lyrebird.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "memwatch.h"

#define MAX_LENGTH 170 /*Maximum length of tweet*/
#define D 1921821779 /*constant for c^d mod n*/
#define N 4294434817 /*constant for c^d mod n*/
#define Debug 0
#define DebugInput 0
void decrypt(FILE* inFile, FILE* outFile){
    /* Takes a specified input file and output file provided by the user
	 * and decrypts the tweet through various functions following the
	 * provided algorithm, where tweet, sumTweet and modTweet are the
	 * modified tweets at different stages of the algorithm. The decrypted
	 * tweet is written to the specified output file
     */
	#if (Debug)
		printf("ENTER LYREBIRD\n");
	#endif
  char inString[MAX_LENGTH];
	char* tweet = NULL;
	unsigned int* sumTweet = NULL;
	unsigned int* modTweet = NULL;
	unsigned int* tweetLength = malloc(sizeof(int));
	if(tweetLength == NULL){
		printf("Could not allocate memory. Exiting.\n");
		exit(1);
	}
    while(fgets(inString, sizeof(inString), inFile) !=NULL){
		*tweetLength = strlen(inString)-1;
 		#if (Debug)
			printf("Tweet length is : %u\n", *tweetLength);
		#endif
		#if (DebugInput)
			FILE* inputFile;
			inputFile = fopen("inputFile.txt","w");
			fprintf(inputFile,"%s", inString);
			fclose(inputFile);
		#endif
		tweet = removeEight(inString, tweetLength);
		int group = *tweetLength/6;
		sumTweet  = (unsigned int*)malloc((group)*sizeof(unsigned int));
		if(sumTweet == NULL){
			printf("Could not allocate memory. Exiting.\n");
			exit(1);
		}
		sumTweet = base41(tweet, sumTweet, tweetLength);
		free(tweet);
		modTweet = (unsigned int*)malloc((group)*sizeof(unsigned int));
		if(modTweet == NULL){
			printf("Could not allocate memory. Exiting.\n");
			exit(1);
		}
		modTweet = cModn(sumTweet, modTweet, tweetLength);
		free(sumTweet);
        invBase41(tweetLength, modTweet, outFile);
		free(modTweet);
		}
	free(tweetLength);
}

char* removeEight(char* tweet,unsigned int* length){
	/* Reads the provided tweet and removes every 8th character. Returns the
	 * modified tweet and updates the length of the tweet
	 */
	#if (Debug)
		printf("ENTER REMOVE 8 PID #%i,\n Tweet is %s, Length is %u\n", getpid(),tweet, *length);
	#endif
	int i;
	unsigned int remCount = 0;
	unsigned int newLength = *length;
	char tempTweet[newLength];
	int counter = 0;
    for (i = 0; tweet[i]!= '\0'; i++ ){
        if(i < *length){
			if ((i+1)%8 != 0){
				tempTweet[counter] = tweet[i];
           		 counter++;
       		 }
			else{
				newLength -=1;
				remCount += 1;
			}
		}
    }
	*length = newLength;
    char* charArray = (char *) malloc((newLength+1)*sizeof(char));
	if (charArray == NULL){
		printf("Could not allocate array in lyrebird, EXITING\n");
		exit(1);
	}
	int k;
	#if (Debug)
		printf("PID# %i Remove 8: Length is : %u, newLength is :%u \n",getpid(), *length, newLength);
	#endif
	for (k = 0; k < newLength; k++){
		charArray[k] = tempTweet[k];
	}
	return charArray;
}

unsigned int* base41(char* tweet,unsigned int* sumTweet, unsigned int* tweetLength){
    /* Takes a tweet as input and groups it into 6 digits in base 41. The sum of these digits
	 * is saved in an array of the modified tweets and returned and the modified tweet length
	 * is updated.
	 */
	#if (Debug)
		printf("ENTER BASE 41");
	#endif
	unsigned int length = *tweetLength;
    char alphabet[41] = {' ','a','b','c','d','e','f','g','h','i','j','k','l',
            'm','n','o','p','q','r','s','t','u','v','w','x','y','z','#','.',',','\'',
            '!','?','(',')','-',':','$','/','&','\\'};
    unsigned int groups = length / 6;
    unsigned int currChar;
	char tweetLetter;
    unsigned int exp;
    unsigned int textPosition;
    unsigned int sum;
    unsigned int currSum;
    int bool;
	int currentGroup;
    for (currentGroup = 0; currentGroup < groups; currentGroup++){
        sum = 0;
        exp = 5;
        currSum = 0;
		int tweetChar;
        for (tweetChar = 0; tweetChar < 6; tweetChar++){
            currChar = currentGroup*6 + tweetChar;
            textPosition = 0;
            bool = 0;
            while (bool != 1){
				tweetLetter = tweet[currChar];
                if (tweetLetter == alphabet[textPosition]){
                    currSum = textPosition * intPower(41,exp);
                    sum += currSum;
                    bool = 1;
                    exp -= 1;
                }
                else{
                    textPosition+= 1;
                }
            }
        }
        sumTweet[currentGroup] = sum;
    }
    *tweetLength = groups;
	return sumTweet;
}


unsigned int* cModn(unsigned int* sumTweet, unsigned int* modTweet, unsigned int* arrayLength){
	/* A funtion that returns the result of (c^d)modn, where d and n are constants.Inputs
	 * the sumTweet and returns anarray with the modified tweet according to the formula
	 */
	unsigned int length = *arrayLength;
    unsigned long long int result;
    unsigned long long int d = D;
	unsigned long long int current;
	int i;
	for (i = 0; i < length; i++){
		current = sumTweet[i];
		result = modExp(current, d);
		modTweet[i] = result;
	}
	return modTweet;
}


unsigned long long int intPower(unsigned long long int base, unsigned long long int exp){
	/*A function for integer exponentation that takes base and exponent and returns base^exp
	 */
	unsigned long long int ret = 1;
    while ( exp != 0 )
    {
    	if ( (exp & 1) == 1 ){
            ret *= base;
		}
        base *= base;
        exp >>= 1;
    }
    return ret;
}

unsigned long long int modExp(unsigned long long int a, unsigned long long int b){
    /*A function for modular exponentiation using exponentiation by squaring and taking mod at
	 * each step of the calculation
	 */
	unsigned long long int n = N;
    unsigned long long int b2 = b/2;
    if (b == 0){
        return 1;
    }
    unsigned long long int bmod2 = Mod(b,2);
    if (bmod2 == 1){
        unsigned long long int tester = modExp(a,b-1) *a;
        return Mod(tester,n);
    }
    else{
        unsigned long long int p = modExp(a,b2);
        return Mod(p*p,n);
    }
}

unsigned long long int Mod(unsigned long long int a, unsigned long long int n){
    /* a function that takes in inputs a and n and returns a mod n
	 */
	unsigned long long int quotient;
    unsigned long long int remainder;
    quotient = a/n;
    remainder = a - (quotient*n);
    return remainder;
}


void invBase41(unsigned int* modArrayLength, unsigned int* modTweet, FILE* outFile){
	/* A function that takes in modTweet and computes the letter vales using our alphabet.
	 * Writes the decrypted result to the output file that was provided by the user.
     */
	unsigned int length = *modArrayLength;
    unsigned int current;
	int exp;
	unsigned int power;
	unsigned int temp;
	unsigned int len = *modArrayLength;
	char tempTweet[len*6];
	int tempCount = 0;
    char alphabet[41] = {' ','a','b','c','d','e','f','g','h','i','j','k','l',
            'm','n','o','p','q','r','s','t','u','v','w','x','y','z','#','.',',','\'',
            '!','?','(',')','-',':','$','/','&','\\'};
	int i;
    for (i = 0; i < length; i++){
        current = modTweet[i];
        for (exp = 5; exp >= 0; exp--){
            power = intPower(41,exp);
            temp = current/power;
            current = current - temp*power;
			tempTweet[tempCount] = alphabet[temp];
			tempCount++;
        }
    }

	for (i = 0; i<len*6;i++){
		fprintf(outFile,"%c",tempTweet[i]);
	}
	fprintf(outFile, "%c", '\n');

}
