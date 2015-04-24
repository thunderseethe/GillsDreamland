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

	int res = mycloud_delfile(req, filename);
	
	//Error checking
	if(res != 0){
		printf("Could not delete file: %s\n", filename);
		return -1;
	}
	printf("Successfully deleted file:%s\n", filename);
	return 0;
}