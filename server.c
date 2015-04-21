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
#include <errno.h>

void processInput(int connfd, int secretKey) {
    size_t size;
    unsigned char *secretKeyBuf = (unsigned char *)malloc(sizeof(char) * 4);
    unsigned char *requestBuf = malloc(sizeof(char) * 4);
    unsigned char *fileSizeBuf = malloc(sizeof(char) * 4);
    unsigned char *fileBuf = malloc(sizeof(char) * 80);

    unsigned int bufferSize = 0; //for EOF
    unsigned char *returnCodeBuf = malloc(sizeof(char) * 4);
    unsigned char *returnSizeCodeBuf = malloc(sizeof(char) * 4);
    rio_t rio;
    Rio_readinitb(&rio, connfd);

    unsigned int nsecretKey, request, fileSize;
    bool shouldUseFilename = false, error = false, willReceiveFile = false, shouldListFiles = false;
    
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
            shouldListFiles = true;
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
        size = Rio_readnb(&rio, fileSizeBuf, 4);
        
        fileSize = char4ToInt(fileSizeBuf);
        
        printf("Creating file buf of size %d\n",fileSize);
        
        char fileContentBuf[fileSize];
        size = Rio_readnb(&rio, fileContentBuf, fileSize);
        
    }
    if (shouldListFiles)
    {
         //for end of file
        DIR *d, *e;
        struct dirent *dir;
        d = opendir("./files");
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                if (dir->d_type == DT_REG)
                {
                    bufferSize += strlen(dir->d_name);
                    bufferSize += 1;
                }
            }
        }
        closedir(d);
        unsigned char *returnListBuf = malloc(sizeof(char) * bufferSize);
        e = opendir("./files");
        int counter = 0;
        if (e)
        {
            while ((dir = readdir(e)) != NULL)
            {
                if (dir->d_type == DT_REG)
                {
                    char temp[strlen(dir->d_name) + 1];
                    printf(temp,"%s\n",dir->d_name);
                    sprintf(temp,"%s\n",dir->d_name);
                    memcpy(&returnListBuf[counter], temp, strlen(temp));
                    counter += strlen(temp);
                    // printf("%s\n", dir->d_name);   
                }
            }
        }
    }
    //all input should be read in now. Now to handle the files themselves

    unsigned int returnCode;
    if(error){
        returnCode = -1;
        printf("Operation Status = error\n");
    } else{
        returnCode = 0;
        printf("Operation Status = success\n");
    }
    intToChar4(returnCode, returnCodeBuf);
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
                printf("Size of files list: %d\n",bufferSize);
                intToChar4(bufferSize, returnSizeCodeBuf);
                Rio_writen(connfd, returnSizeCodeBuf, 4);
                break;
            default:
                break;
        }
    }
    
    //rio_readn(int fd, void *usrbuf, size_t n)
    //while((n = Rio_readn(&rio, buf, MAXLINE)) != 0) { //line:netp:echo:eof
    //  printf("server received %d bytes\n", n);
    //send the response
    //  Rio_writen(connfd, buf, n);
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
    //if the directory couldn't be created and doesn't already exist
    if (status == -1 && errno != EEXIST)
    {
        perror("mkdir");
    }
    //TODO: clear the directory of files (because we aren't supposed to keep them?)



    port = atoi(argv[1]);
    secretKey = atoi(argv[2]);

    listenfd = Open_listenfd(port);
    while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t*)&clientlen);

    /* determine the domain name and IP address of the client */
    // hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
    //         sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    // haddrp = inet_ntoa(clientaddr.sin_addr);
    // printf("server connected to %s (%s)\n", hp->h_name, haddrp);
    
    processInput(connfd, secretKey);

    Close(connfd);
    }
    
    exit(0);
}
/* $end echoserverimain */
