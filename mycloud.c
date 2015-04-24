#include "mycloud.h"

int create_req(unsigned char* request, int req_len, unsigned long hKey, unsigned long hType, char* filename, unsigned long hSize, unsigned char* data){
	//Invalid Type check
	if(hType < 0 || hType > 4){
		printf("Type: %lu, Invalid type", hType);
		return -1;
	}
	int filename_length = strlen(filename);
	//Invalid filename length check
	if(filename_length > 80){
		printf("Filename: %s, is an invalid length", filename);
		return -1;
	}
	//Convert the numbers to network byte order
	unsigned long key = htonl(hKey);
	unsigned long type = htonl(hType);
	unsigned long nSize = htonl(hSize);

	//Loop through and build char* request case by case
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

//Wrappers for create_req
int build_put_req(unsigned char* request, unsigned long hKey, char* filename, unsigned long hSize, unsigned char* data){
	return create_req(request, 92+hSize, hKey, 1, filename, hSize, data);
}
int build_get_req(unsigned char* request, unsigned long hKey, char* filename){
	return create_req(request, 88, hKey, 0, filename, 0, (unsigned char*)"");
}
int build_del_req(unsigned char* request, unsigned long hKey, char* filename){
	return create_req(request, 88, hKey, 2, filename, 0, (unsigned char*)"");
}
int build_list_req(unsigned char* request, unsigned long hKey){
	return create_req(request, 8, hKey, 3, "", 0, (unsigned char*)"");
}

int open_client(Request req){
	if(strlen(req.hostname) < 0){
		printf("Invalid Hostname: %s\n", req.hostname);
		return -1;
	}
	if(req.port < 0 || req.port > 65535){
		printf("Invalid Port(0 - 65535): %d", req.port);
		return -1;
	}
	if(req.secretkey <= 0){
		printf("Invalid Key: %lu", req.secretkey);
		return -1;
	}
	int clientfd = Open_clientfd(req.hostname, req.port);
	return clientfd;
}

//Put request api call
int mycloud_putfile(Request server, char *filename, unsigned char *data, int size){
	int clientfd;
	int req_len = 92 + size;
	int resp_len = 4;
	unsigned char* buf = malloc(sizeof(unsigned char) * resp_len);
	
	/* BUILD REQUEST */
	unsigned char* request = malloc(req_len*sizeof(unsigned char));
	int result = build_put_req(request, server.secretkey, filename, (unsigned long)size, data);
	if(result == -1){
		return -1;
	}
	//Open socket, write request, read request
	rio_t rio;
	clientfd = open_client(server);
	if(clientfd == -1){
		return -1;
	}
	Rio_readinitb(&rio, clientfd);
	Rio_writen(clientfd, request, req_len);
	Rio_readnb(&rio, buf, resp_len);

	//Convert buffer to int status
	int status = char4ToInt(buf);
	if(status != 0){
		return -1;
	}
	return 0;
}

//Get request api call
int mycloud_getfile(Request server, char *filename, unsigned char *data, int data_length){
	int clientfd;
	int req_len = 88;
	unsigned char* buf = malloc(4*sizeof(unsigned char));

	/* BUILD REQUEST */
	unsigned char* request = malloc(req_len*sizeof(unsigned char));
	int result = build_get_req(request, server.secretkey, filename);
	if(result == -1){
		return -1;
	}

	//Open socket, write request
	rio_t rio;
	clientfd = open_client(server);
	if(clientfd == -1){
		return -1;
	}
	Rio_readinitb(&rio, clientfd);
	Rio_writen(clientfd, request, req_len);
	
	//Convert buffer to int status
	Rio_readnb(&rio, buf, 4);
	int status = char4ToInt(buf);
	if(status != 0){
		return -1;
	}

	//Convert buffer to int size
	Rio_readnb(&rio, buf, 4);
	int size = char4ToInt(buf);
	if(data_length < size){
		size = data_length;
	}

	//If size is bigger then our buffer handle it with a for loop
	if(size < MAXLINE){
		Rio_readnb(&rio, data, size);
	}
	else{
		for(int i = 0; i < size; i += MAXLINE){
			Rio_readnb(&rio, data+i, MAXLINE);
		}
	}
	return size;
}

int mycloud_delfile(Request server, char* filename){
	int clientfd;
	int req_len = 88;
	unsigned char* buf = malloc(4*sizeof(unsigned char));

	/* BUILD REQUEST */
	unsigned char* request = malloc(req_len*sizeof(unsigned char));
	int result = build_del_req(request, server.secretkey, filename);
	if(result == -1){
		return -1;
	}

	//Open socket, write request
	rio_t rio;
	clientfd = open_client(server);
	if(clientfd == -1){
		return -1;
	}
	Rio_readinitb(&rio, clientfd);
	Rio_writen(clientfd, request, req_len);

	//Convert buffer to int status
	Rio_readnb(&rio, buf, 4);
	int status = char4ToInt(buf);
	if(status != 0){
		return -1;
	}
	return 0;
}

int mycloud_listfiles(Request server, unsigned char* listbuf, int list_length){
	int clientfd;
	int req_len = 8;
	unsigned char* buf = malloc(4);

	/* BUILD REQUEST */
	unsigned char* request = malloc(req_len*sizeof(unsigned char));
	int result = build_list_req(request, server.secretkey);
	if(result == -1){
		printf("Result could not be built\n");
		return -1;
	}

	//Open socket, write request
	rio_t rio;
	clientfd = open_client(server);
	if(clientfd == -1){
		printf("Client could not be reached");
		return -1;
	}
	Rio_readinitb(&rio, clientfd);
	Rio_writen(clientfd, request, req_len);
	
	//Convert buffer to int status
	Rio_readnb(&rio, buf, 4);
	int status = char4ToInt(buf);
	if(status != 0){
		printf("Status is %d", status);
		return -1;
	}
	Rio_readnb(&rio, buf, 4);
	int size = char4ToInt(buf);
	if(list_length < size){
		size = list_length;
	}

	//If size is bigger then our buffer handle it with a for loop
	if(size < MAXLINE){
		Rio_readnb(&rio, listbuf, size);
	}
	else{
		for(int i = 0; i < size; i += MAXLINE){
			Rio_readnb(&rio, listbuf+i, MAXLINE);
		}
	}
	return 0;
}