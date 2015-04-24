#include <stdio.h>
#include <stdlib.h>
#include "mycloud.h"

int main(int argc, char* argv[]){
	if(argc < 5){
		printf("Usage: %s <host> <port> <secret key> <filename>\n", argv[0]);
		return -1;
	}
	//Build server info
	Request req;
	req.hostname = argv[1];
	req.port = atoi(argv[2]);
	req.secretkey = atol(argv[3]);
	char* filename = argv[4];
	//Max file size of 100kB
	int size = 102400;
	unsigned char* data = malloc(size*sizeof(unsigned char));
	//Read from stdin till EOF
	int read = fread(data, sizeof(unsigned char), size , stdin);
	int res = mycloud_putfile(req, filename, data, read);
	//Error checking
	if(res != 0){
		printf("Could not put file: %s\n", filename);
		return -1;
	}
	printf("Succesfully put file: %s\n", filename);
	return 0;
}