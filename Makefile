CFLAGS = -Wall -std=gnu99
CPPFLAGS = -Wall

all: server client

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c

util.o: util.c
	gcc $(CFLAGS) util.c -c

server: server.cpp csapp.h csapp.o util.o
	g++ $(CPPFLAGS) server.cpp csapp.o util.o -lpthread -o mycloud_server

mycloud.o: mycloud.c
	gcc $(CFLAGS) mycloud.c -c -o mycloud.o

mycloud: mycloud.o csapp.o util.o
	gcc $(CFLAGS) mycloud.o csapp.o util.o -lpthread -o mycloud

client: mcput mcget mcdel mclist

mcput: mcput.c csapp.o mycloud.o util.o
	gcc $(CFLAGS) mcput.c mycloud.o util.o csapp.o -lpthread -o mcput

mcget: mcget.c csapp.o mycloud.o util.o
	gcc $(CFLAGS) mcget.c mycloud.o util.o csapp.o -lpthread -o mcget

mcdel: mcdel.c csapp.o mycloud.o util.o
	gcc $(CFLAGS) mcdel.c mycloud.o util.o csapp.o -lpthread -o mcdel

mclist: mclist.c csapp.o mycloud.o util.o
	gcc $(CFLAGS) mclist.c mycloud.o util.o csapp.o -lpthread -o mclist

.PHONY: clean
clean:
	/bin/rm -rf csapp.h csapp.c *.o mycloud_server mcput mcget mcdel mclist mycloud
