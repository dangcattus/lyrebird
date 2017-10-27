#the compiler that we are using

##CC = gcc
CC = g++
#compiler flags
#add -ansi if need be, but not cimpiling netdb.h
CFLAGS = -Wall -ansi -DMEMWATCH -DMW_STDIO

#all is the default target to build
all: lyrebird.server lyrebird.client

lyrebird.client: client.o memwatch.o fcfs.o processHelper.o lyrebird.o
		$(CC) $(CFLAGS) -o lyrebird.client client.o memwatch.o fcfs.o processHelper.o lyrebird.o
#this creates the object files
lyrebird.server: lyrebird.o memwatch.o main.o processHelper.o fcfs.o socketHelper.o
		$(CC) $(CFLAGS) -o lyrebird.server lyrebird.o memwatch.o main.o processHelper.o fcfs.o socketHelper.o

client.o: client.c memwatch.c memwatch.h fcfs.c fcfs.h processHelper.h processHelper.c lyrebird.c lyrebird.h
		$(CC) $(CFLAGS) -c client.c memwatch.c fcfs.c processHelper.c lyrebird.c

#source files to create object files
lyrebird.o: main.c lyrebird.c memwatch.c lyrebird.h processHelper.c processHelper.h fcfs.c fcfs.h socketHelper.c socketHelper.h
		$(CC) $(CFLAGS) -c main.c lyrebird.c memwatch.c processHelper.c fcfs.c socketHelper.c


#make everything clean
clean:
		$(RM) lyrebird lyrebird.server lyrebird.client memwatch.log *.o *.~
