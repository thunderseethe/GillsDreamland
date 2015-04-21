CFLAGS = -Wall
CPPFLAGS = -Wall

all: server mcput mcget mcdel mclist

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c

util.o:
	gcc $(CFLAGS) -std=c99 util.c -c

server: server.c csapp.o util.o
	gcc $(CPPFLAGS) server.c csapp.o util.o -lpthread -o server -std=c99

mycloud: mycloud.c csapp.o util.o
	gcc $(CPPFLAGS) mycloud.c csapp.o util.o -lpthread -o mycloud -std=c99

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
