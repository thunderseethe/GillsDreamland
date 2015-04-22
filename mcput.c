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
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		printf("Could not read file: %s", filename);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* data = malloc(size*sizeof(char));
	fread(data, size, sizeof(char), fp);
	int res = mycloud_putfile(req, filename, data, size);
	if(res != 0){
		printf("Could not put file: %s\n", filename);
		return -1;
	}
	return 0;
}