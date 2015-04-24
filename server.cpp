// CS 485G Project 5
// Ethan Gill and Ethan Smith
// Due April 24, 2015
// server.cpp
// This is a simple cloud server. It supports creation, deletion, listing, and sending files over TCP.
// For more information, view the README
// make: 'make server'
#include "server.h"
#include "csapp.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

extern "C" {
    #include "util.h"
}

void processInput(int connfd, unsigned int secretKey) {
    size_t size;
    //buffers required by all commands
    unsigned char *secretKeyBuf = (unsigned char*)malloc(sizeof(char) * 4);
    unsigned char *requestBuf = (unsigned char*)malloc(sizeof(char) * 4);

    //get and put
    unsigned char *fileSizeBuf = (unsigned char*)malloc(sizeof(char) * 4);
    char *fileNameBuf = (char*)malloc(sizeof(char) * 80);
    char *fileContentsBuf;

    //list buffers
    unsigned char *returnListBuf;

    //return buffers
    unsigned char *returnCodeBuf = (unsigned char*)malloc(sizeof(char) * 4);
    unsigned char *returnSizeCodeBuf = (unsigned char*)malloc(sizeof(char) * 4);

    //RIO stuff
    rio_t rio;
    Rio_readinitb(&rio, connfd);

    //ints parsed from buffers
    unsigned int nsecretKey, request, fileSize;
    unsigned int bufferSize = 0; //for EOF
    //bools used for method logic (not necessary, but helps with reading the file)
    bool shouldUseFilename = false, error = false, list = false, get = false, put = false, del = false;
    
    //read in the secret key and type of request.
    size = Rio_readnb(&rio, secretKeyBuf, 4);
    if(size!=4){
        error = true;
    }
    size = Rio_readnb(&rio, requestBuf, 4);
    if(size!=4){
        error = true;
    }

    //convert the secret key and request buffers to ints
    nsecretKey = char4ToInt(secretKeyBuf);
    request = char4ToInt(requestBuf);
    printf("Secret Key = %d\n", nsecretKey);
    printf("Request Type = ");

    //check the secret keys and set error
    if (nsecretKey != secretKey)
    {
        error = true;
    }

    //print some stuff to the console depending on what command is requested
    switch(request) {
        case 0:
            //get
            printf("get\n");
            printf("Filename = ");
            shouldUseFilename = true;
            get = true;
            break;
        case 1:
            //put
            printf("put\n");
            printf("Filename = ");
            shouldUseFilename = true;
            put = true;
            break;
        case 2:
            //del
            printf("del\n");
            printf("Filename = ");
            shouldUseFilename = true;
            del = true;
            break;
        case 3:
            //list
            printf("list\n");
            printf("Filename = NONE\n");
            list = true;
            break;
        default:
            printf("undefined: %d\n",request);
            printf("Filename = NONE\n");
            error = true;
            break;
    }
    //if the command we're utilizing receives a filename, read it in.
    if (shouldUseFilename) {
        size = Rio_readnb(&rio, fileNameBuf, 80);
        printf("%s\n",fileNameBuf);
    }
    if (put && !error) {
        size = Rio_readnb(&rio, fileSizeBuf, 4);
        fileSize = char4ToInt(fileSizeBuf);
        //printf("Creating file buf of size %d\n",fileSize);
        fileContentsBuf = (char*)malloc(sizeof(char) * (fileSize+1));
        size = Rio_readnb(&rio, fileContentsBuf, fileSize);

        FILE *stream;
        char filePath[100];
        //create the file path
        strcpy(filePath, "./files/");
        strcat(filePath, fileNameBuf);
        if ((stream = fopen(filePath, "w")))
        {
            //write the file
            fwrite(fileContentsBuf, 1, fileSize, stream);
            //Debug print
            //printf("Write: %s\n",fileContentsBuf);
            fclose(stream);
        } else {
            //an error occurred writing the file
            perror("Put file");
            error = true;
        }
    }
    if (list && !error)
    {
         //for end of file
        DIR *d, *e;
        struct dirent *dir;
        //open the files directory
        d = opendir("./files");
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                if (dir->d_type == DT_REG)
                {
                    //count up the length of each filename (for the length byte)
                    bufferSize += strlen(dir->d_name);
                    bufferSize += 1;
                }
            }
        }
        closedir(d);
        returnListBuf = (unsigned char*)malloc(sizeof(char) * bufferSize);
        e = opendir("./files");
        int counter = 0;
        if (e)
        {
            while ((dir = readdir(e)) != NULL)
            {
                if (dir->d_type == DT_REG)
                {
                    //print each name to a buffer
                    //printf("%s\n",dir->d_name);
                    char temp[strlen(dir->d_name) + 1];
                    sprintf(temp,"%s\n",dir->d_name);
                    memcpy(&returnListBuf[counter], temp, strlen(temp));
                    counter += strlen(temp);
                    // printf("%s\n", dir->d_name);   
                }
            }
        }
    }
    if(get && !error){
        FILE *stream;
        fileSize = 0;
        char filePath[100];
        strcpy(filePath, "./files/");
        strcat(filePath, fileNameBuf);
        //try to open the file (in the files directory)
        if ((stream = fopen(filePath, "rb")))
        {
            //Seek to the end of the file to determine the file size
            fseek(stream, 0L, SEEK_END);
            fileSize = ftell(stream);
            fseek(stream, 0L, SEEK_SET);
            //printf("fileSize: %d\n",fileSize);
            fileContentsBuf = (char*)malloc(sizeof(char) * (fileSize+1));
            size=fread(fileContentsBuf,1,fileSize,stream);
            fileContentsBuf[size]=0; //add EOF
            //Debug print
            //printf("Read: %s\n",fileContentsBuf);
            //printf("Size T: %lu\n", size);
            fclose(stream);
        } else {
            //an error occurred opening the file. It may not exist, or permissions aren't set
            perror("Open file");
            error = true;
        }
    }
    if(del && !error){
        char filePath[100];
        strcpy(filePath, "./files/");
        strcat(filePath, fileNameBuf);
        //delete the file at path
        //printf("deleting file at path: %s\n",filePath);
        int delResult = unlink(filePath);
        if(delResult != 0){
            error = true;
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
    //printf("sent\n");

    //only send the get and list info if there isn't an error
    if(!error){
        switch(request) {
            case 0:
                //get
                //bufferSize = strlen(fileContentsBuf) + 1;
                //printf("Size of file: %lu\n",size);
                intToChar4(size, returnSizeCodeBuf);
                Rio_writen(connfd, returnSizeCodeBuf, 4);
                Rio_writen(connfd, fileContentsBuf, size);
                // for (unsigned int i = 0; i < size; ++i)
                // {
                //     printf("%02x",fileContentsBuf[i]);
                // }
                break;
            case 3:
                //list
                //printf("Size of files list: %d\n",bufferSize);
                intToChar4(bufferSize, returnSizeCodeBuf);
                Rio_writen(connfd, returnSizeCodeBuf, 4);
                Rio_writen(connfd, returnListBuf, bufferSize);
                break;
            default:
                break;
        }
    }

    printf("--------------------------\n");
}

int main(int argc, char **argv) 
{
    int listenfd, connfd, port, clientlen; 
    unsigned int secretKey;
    struct sockaddr_in clientaddr;
    int status; //for dir commands
    //struct hostent *hp;
    //char *haddrp;
    
    //wrong arguments
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
    //get the port and secret key
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
