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
	int length = MAXLINE;
	char* data = malloc(length*sizeof(char));
	int res = mycloud_getfile(req, filename, data, length);
	if(res == -1){
		printf("Could not get file: %s\n", filename);
		return -1;	
	} 
	FILE *fp = fopen(filename, "w");
	if(fp == NULL){
		printf("Can't open file: %s\n", filename);
		exit(1);
	}
	fprintf(fp, "%s", data);
	fclose(fp);
	return 0;
}