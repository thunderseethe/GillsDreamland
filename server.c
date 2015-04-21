/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "server.h"
#include "csapp.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

void processInput(int connfd, int secretKey) {
    size_t size;
    unsigned char *secretKeyBuf = malloc(sizeof(char) * 4);
    unsigned char *requestBuf = malloc(sizeof(char) * 4);
    unsigned char *fileBuf = malloc(sizeof(char) * 80);
    unsigned char *returnCodeBuf = malloc(sizeof(char) * 4);
    unsigned char *sizeCodeBuf = malloc(sizeof(char) * 4);
    rio_t rio;
    Rio_readinitb(&rio, connfd);

    //unsigned int fileSizeNet;
    unsigned int nsecretKey, request;
    //unsigned int fileSize;
    bool shouldUseFilename = false, error = false, willReceiveFile = false;
    
    //read in the secret key and type of request.
    size = Rio_readnb(&rio, secretKeyBuf, 4);
    if(size!=4){
        error = true;
    }
    size = Rio_readnb(&rio, requestBuf, 4);
    if(size!=4){
        error = true;
    }

    nsecretKey = char4ToInt(secretKeyBuf);
    request = char4ToInt(requestBuf);
    printf("Secret Key = %d\n", nsecretKey);
    printf("Request Type = ");

    //check the secret keys and set error
    if (nsecretKey != secretKey)
    {
        error = true;
    }

    switch(request) {
        case 0:
            //get
            printf("get\n");
            printf("Filename = ");
            shouldUseFilename = true;
            break;
        case 1:
            //put
            printf("put\n");
            printf("Filename = ");
            shouldUseFilename = true;
            willReceiveFile = true;
            break;
        case 2:
            //del
            printf("del\n");
            printf("Filename = ");
            shouldUseFilename = true;
            break;
        case 3:
            //list
            printf("list\n");
            printf("Filename = NONE\n");
            break;
        default:
            printf("undefined: %d\n",request);
            printf("Filename = NONE\n");
            error = true;
            break;
    }
    if (shouldUseFilename) {
        size = Rio_readnb(&rio, fileBuf, 80);
        printf("%s\n",fileBuf);
    }
     if (willReceiveFile) {
    // 	size = Rio_readnb(&rio, fileSizeBuf, 4);
    	
    	
    // 	fileSizeNet = *((int *) &fileSizeBuf[0]);
    // 	fileSize = ntohl(fileSizeNet);
    	
    // 	//printf("Creating file buf of size %d\n",fileSize);
    	
    // 	char fileContentBuf[fileSize];
    // 	size = Rio_readnb(&rio, fileContentBuf, fileSize);
    	
     }
    //all input should be read in now. Now to handle the files themselves

    unsigned int returnCode;
    unsigned int sizeCode;
    if(error){
        returnCode = -1;
        printf("Operation Status = error\n");
    } else{
        returnCode = 0;
        printf("Operation Status = success\n");
    }
    intToChar4(returnCode, returnCodeBuf);
    for (int i = 0; i < 4; ++i)
                {
                    printf("%d ",returnCodeBuf[i]);
                }
    Rio_writen(connfd, returnCodeBuf, 4);

    if(!error){
        switch(request) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                //list
                sizeCode = 0;
                intToChar4(sizeCode, sizeCodeBuf);
                Rio_writen(connfd, sizeCodeBuf, 4);
                break;
            default:
                break;
        }
    }
    
    //rio_readn(int fd, void *usrbuf, size_t n)
    //while((n = Rio_readn(&rio, buf, MAXLINE)) != 0) { //line:netp:echo:eof
    //	printf("server received %d bytes\n", n);
    //send the response
    //	Rio_writen(connfd, buf, n);
    //}
}

int main(int argc, char **argv) 
{
    int listenfd, connfd, port, clientlen, secretKey;
    struct sockaddr_in clientaddr;
    int status; //for dir commands
    //struct hostent *hp;
    //char *haddrp;
    
    if (argc != 3) {
	fprintf(stderr, "usage: %s <port> <secretKey>\n", argv[0]);
	exit(0);
    }

    //setup the current working directory, using subfolder 'files'
    //if the folder doesn't exist, create it
    status = mkdir("files", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    printf("%d\n",status);
    if (status == -1)
    {
        perror("mkdir");
    }

    port = atoi(argv[1]);
    secretKey = atoi(argv[2]);

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t*)&clientlen);

	/* determine the domain name and IP address of the client */
	// hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
	// 		   sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	// haddrp = inet_ntoa(clientaddr.sin_addr);
	// printf("server connected to %s (%s)\n", hp->h_name, haddrp);
    
    processInput(connfd, secretKey);

	Close(connfd);
    }
    
    exit(0);
}
/* $end echoserverimain */
