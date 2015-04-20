#include "mycloud.h"

int create_req(unsigned char* request, int req_len, unsigned long hKey, unsigned long hType, char* filename, unsigned long hSize, char* data){
	if(hType < 0 || hType > 4){
		return -1;
	}
	int filename_length = strlen(filename);
	if(filename_length > 80){
		return -1;
	}
	unsigned long key = htonl(hKey);
	unsigned long type = htonl(hType);
	unsigned long nSize = htonl(hSize);
	for(int i = 0; i < req_len; i++){
		//0-3: secret key
		if(i <= 3){
			request[i] = (key >> (3-i)*8) & 0xFF;
		}
		//4-7: request type, get(0), put(1), del(2), list(3)
		else if(i <= 7){
			request[i] = (type >> (7-i)*8) & 0xFF;
		}
		//8-87: filename, fill with \0 once the filename is copied over
		else if(i <= 87){
			if((i-8) < filename_length)
				request[i] = filename[i-8];
			else
				request[i] = '\0';
		}
		//88-91: number of bytes in the file
		else if(i <= 91){
			request[i] = (nSize >> (91 - i)*8) & 0xFF;
		}
		//92+: file data of length specified by bytes 87-91
		else{
			request[i] = data[i - 92];
		}
	}
	return 0;
}

int build_put_req(unsigned char* request, unsigned long hKey, char* filename, unsigned long hSize, char* data){
	return create_req(request, 92+hSize, hKey, 1, filename, hSize, data);
}
int build_get_req(unsigned char* request, unsigned long hKey, char* filename){
	return create_req(request, 88, hKey, 0, filename, 0, "");
}
int build_del_req(unsigned char* request, unsigned long hKey, char* filename){
	return create_req(request, 88, hKey, 2, filename, 0, "");
}
int build_list_req(unsigned char* request, unsigned long hKey){
	return create_req(request, 8, hKey, 3, "", 0, "");
}

int mycloud_putfile(Request server, char *filename, char *data, int size){
	int clientfd;
	int req_len = 92 + size;
	int resp_len = 4;
	char* buf;
	
	/* BUILD REQUEST */
	unsigned char* request = malloc(req_len*sizeof(unsigned char));
	int result = build_put_req(request, server.secretkey, filename, (unsigned long)size, data);
	if(result == -1){
		return -1;
	}
	rio_t rio;
	clientfd = Open_clientfd(server.hostname, server.port);
	Rio_readinitb(&rio, clientfd);
	Rio_writen(clientfd, request, req_len);
	Rio_readnb(&rio, buf, resp_len);
	unsigned long nStatus = 0;
	for(int i = 0; i < resp_len; i++){
		nStatus += request[i] * (1 << (3 - i)*2);
	}
	printf("status: %lx", nStatus);
	if(nStatus != 0){
		return -1;
	}
	return 0;
}
int mycloud_getfile(Request server, char *filename, char *data, int data_length){
	int clientfd;
	int req_len = 88;
	char* buf;
	unsigned char* request = malloc(req_len*sizeof(unsigned char));
	int result = build_get_req(request, server.secretkey, filename);
	if(result == -1){
		return -1;
	}

	rio_t rio;
	clientfd = Open_clientfd(server.hostname, server.port);
	Rio_readinitb(&rio, clientfd);
	Rio_writen(clientfd, request, req_len);
	Rio_readnb(&rio, buf, 4);
	unsigned long nStatus = 0;
	for(int i = 0; i < 4; i++){
		nStatus += buf[i] * (1 << i*2);
	}
	unsigned long status = ntohl(nStatus);
	if(status != 0){
		return -1;
	}
	Rio_readnb(&rio, buf, 4);
	unsigned long nSize = 0;
	for(int i = 0; i < 4; i++){
		nSize += buf[i] * (1 << i*2);
	}
	unsigned long size = htonl(nSize);
	if(data_length < size){
		size = data_length;
	}
	if(size < MAXLINE){
		Rio_readnb(&rio, buf, size);
		strcat(data, buf);
	}
	else{
		for(int i = 0; i < size; i += MAXLINE){
			Rio_readnb(&rio, buf, i);
			strcat(data, buf);
		}
	}
	/*printf("0:\t");
	for(int i = 0; i < req_len; i++){
		printf("%02X\t", request[i]);
		if(((i+1) % 4) == 0){
			printf("\n%d:\t", i+1);
		}
	}
	printf("\n");*/
	return size;
}
int mycloud_delfile(Request server, char* filename){
	int clientfd;
	int req_len = 88;
	char *buf;
	unsigned char* request = malloc(req_len*sizeof(unsigned char));
	int result = build_del_req(request, server.secretkey, filename);
	if(result == -1){
		return -1;
	}
	rio_t rio;
	clientfd = Open_clientfd(server.hostname, server.port);
	Rio_writen(clientfd, request, req_len);
	Rio_readnb(&rio, buf, 4);
	unsigned long nStatus = 0;
	for(int i = 0; i < 4; i++){
		nStatus += buf[i] * (1 << i*2);
	}
	unsigned long status = ntohl(nStatus);
	if(status != 0){
		return -1;
	}
	
	return 0;
}
int mycloud_listfiles(Request server, char* listbuf, int list_length){
	int clientfd;
	int req_len = 8;
	unsigned char* buf = malloc(4);
	unsigned char* request = malloc(req_len*sizeof(unsigned char));
	int result = build_list_req(request, server.secretkey);
	if(result == -1){
		return -1;
	}

	rio_t rio;
	printf("0:\t");
	for(int i = 0; i < req_len; i++){
		printf("%02X\t", request[i]);
		if(((i+1) % 4) == 0){
			printf("\n%d:\t", i+1);
		}
	}
	printf("\n");

	clientfd = Open_clientfd(server.hostname, server.port);
	Rio_readinitb(&rio, clientfd);
	Rio_writen(clientfd, request, req_len);
	Rio_readnb(&rio, buf, 4);
	unsigned long nStatus = 0;
	printf("buf = ");
	for(int i = 0; i < 4; i++){
		printf("%02x\t", buf[i]);
		nStatus += buf[i] * (1 << i*2);
	}
	printf("\nstatus: %lx\n", nStatus);
	unsigned long status = ntohl(nStatus);
	return 0;
	if(status != 0){
		return -1;
	}
	Rio_readnb(&rio, buf, 4);
	unsigned long nSize = 0;
	for(int i = 0; i < 4; i++){
		nSize += buf[i] * (1 << i*2);
	}
	unsigned long size = htonl(nSize);
	if(list_length < size){
		size = list_length;
	}
	if(size < MAXLINE){
		Rio_readnb(&rio, buf, size);
		strcat(listbuf, buf);
	}
	else{
		for(int i = 0; i < size; i += MAXLINE){
			Rio_readnb(&rio, buf, size);
			strcat(listbuf, buf);
		}
	}
	/*printf("0:\t");
	for(int i = 0; i < req_len; i++){
		printf("%02X\t", request[i]);
		if(((i+1) % 4) == 0){
			printf("\n%d:\t", i+1);
		}
	}
	printf("\n");*/
	return 0;
}
int main(int argc, char** argv){
	Request server;
	server.hostname = "192.168.77.232";
	server.port = 1234;
	server.secretkey = 12345;
	//mycloud_getfile(server, "test.txt", "file data goes here", 19);
	char* list = "empty list";
	mycloud_listfiles(server, list, 100);
	printf("%s\n", list);
	return 0;
}