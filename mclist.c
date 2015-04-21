#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "mycloud.h"

int main(int argc, char* argv[]){
	if(argc < 3){
		printf("Usage: %s MachineName TCPPort SecretKey\n", argv[0]);
		return -1;
	}
	Request req;
	req.hostname = argv[1];
	req.port = atoi(argv[2]);
	req.secretkey = atol(argv[3]);
	int list_length = MAXLINE;
	char* list = (char*)malloc(MAXLINE*sizeof(char));
	mycloud_listfiles(req, list, list_length);
	printf("File List\n-----------\n%s", list);
}