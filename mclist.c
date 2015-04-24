#include <stdio.h>
#include <stdlib.h>
#include "mycloud.h"

int main(int argc, char* argv[]){
	//Check appropiate command usage
	//We don't mind if you have more then 4, you just need atleast 4
	if(argc < 4){
		printf("Usage: %s MachineName TCPPort SecretKey\n", argv[0]);
		return -1;
	}
	//Build our server info
	Request req;
	req.hostname = argv[1];
	req.port = atoi(argv[2]);
	req.secretkey = atol(argv[3]);

	//Arbitrary max on list length
	int list_length = MAXLINE;
	unsigned char* list = (unsigned char*)malloc(MAXLINE*sizeof(unsigned char));
	int res = mycloud_listfiles(req, list, list_length);
	if(res == -1){
		printf("Could not get list of files\n");
		return -1;
	}
	printf("%s", list);
	return 0;
}