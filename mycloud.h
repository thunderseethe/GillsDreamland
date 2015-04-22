#ifndef __MYCLOUD_H__
#define __MYCLOUD_H__
#include "csapp.h"
#include "util.h"
#include <netdb.h>
#include <string.h>
#include <stdio.h>

typedef struct {
	char *hostname;
	int port;
	unsigned long secretkey;
} Request;


int mycloud_putfile(Request server, char *filename, char *data, int size);
int mycloud_getfile(Request server, char *filename, char *data, int size);
int mycloud_delfile(Request server, char* filename);
int mycloud_listfiles(Request server, char* listbuf, int length);

#endif