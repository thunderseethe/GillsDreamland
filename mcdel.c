#include <stdio.h>
#include <stdlib.h>
#include "mycloud.h"

int main(int argc, char* argv[]){
	if(argc < 5){
		printf("Usage: %s MachineName TCPport SecretKey filename", argv[0]);
	}
	Request req;
	req.hostname = argv[1];
	req.port = atoi(argv[2]);
	req.secretkey = atol(argv[3]);
	char* filename = argv[4];

	printf("del: %s\n", filename);
	int res = mycloud_delfile(req, filename);
	printf("result = %d", res);
	if(res != 0){
		printf("Could not delete file: %s", filename);
		return -1;
	}
	return 0;
}