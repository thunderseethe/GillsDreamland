CFLAGS = -Wall
CPPFLAGS = -Wall

all: server mcput mcget mcdel mclist

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csappcpp.h:
	wget http://www.cs.uky.edu/~raphael/courses/CS485/proj5/csappcpp.h

csapp.o: csapp.h csapp.c

util.o: util.c
	gcc $(CFLAGS) -std=c99 util.c -c

server: server.cpp csappcpp.h csapp.o util.o
	g++ $(CPPFLAGS) server.cpp csapp.o util.o -lpthread -o server

mycloud: mycloud.c csapp.o util.o
	gcc $(CFLAGS) mycloud.c csapp.o util.o -lpthread -o mycloud -std=c99

mcput: mcput.cpp csapp.o
	g++ $(CPPFLAGS) mcput.cpp csapp.o -lpthread -o mcput

mcget: mcget.cpp csapp.o
	g++ $(CPPFLAGS) mcget.cpp csapp.o -lpthread -o mcget

mcdel: mcdel.cpp csapp.o
	g++ $(CPPFLAGS) mcdel.cpp csapp.o -lpthread -o mcdel

mclist: mclist.cpp csapp.o
	g++ $(CPPFLAGS) mclist.cpp csapp.o -lpthread -o mclist

.PHONY: clean
clean:
	/bin/rm -rf csapp.h csapp.c *.o server mcput mcget mcdel mclist mycloud
