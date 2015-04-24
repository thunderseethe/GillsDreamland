#include <stdio.h>
#include <stdlib.h>
#include "mycloud.h"

int main(int argc, char* argv[]){
	if(argc < 5){
		printf("Usage: %s <host> <port> <secret key> <filename>\n", argv[0]);
		return -1;
	}
	//Build request server info
	Request req;
	req.hostname = argv[1];
	req.port = atoi(argv[2]);
	req.secretkey = atol(argv[3]);
	char* filename = argv[4];

	//Max file size is 100kB
	int length = 102400;
	unsigned char* data = malloc(length*sizeof(unsigned char));
	int res = mycloud_getfile(req, filename, data, length);
	//Error checking
	if(res == -1){
		printf("File not found: %s\n", filename);
		return -1;	
	}
	//Print file to stdout
	for(int i = 0; i < res; i++){
		printf("%c", data[i]);
	}
	return 0;
}