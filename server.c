//CAUTION!!! THIS CODE IS WRITTEN IN C99, TO COMPILE IT WITH COMMAND LINE, YOU NEED TO USE -std=c99
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "macros.h"
#include "structs.h"
#include "messages.h"
#include "threads.h"

int main(char argc, char* argv[]){
    //Initializing connected client list
    client_info             list[MAX_USERS];    //try with dynamic allocation
    //declaration of server elements
    int                     lsock;  //listening socket
    int                     csock;  //connection socket
    struct sockaddr_in      saddr;  //server address
    struct sockaddr_in      caddr;  //client address
    int                     csize;  //client address size
    //BUILDING SERVER CONNECTION STRUCTURE
    //socket operation
    if(((lsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) < 0) {
        printf("ERROR on socket operation.\n");
        exit(EXIT_FAILURE);
    }
    //Initializing connected client list
    memset(&list, 0, sizeof(list));
    for(int i = 0; i < MAX_USERS; i++) {
        list[i].sock = -1;
    }
    //Initializing server elements
    memset(&saddr, 0, sizeof(saddr));
    memset(&caddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    if(argc > 1) saddr.sin_port = htons(atoi(argv[1])); else saddr.sin_port = htons(DEFAULT_PORT);
    //bind operation
    if(bind(lsock, (struct sockaddr*)&saddr, sizeof(saddr)) < 0){
        printf("ERROR on bind operation.\n");
        exit(EXIT_FAILURE);
    }
    //listen operation
    listen(lsock, MAX_CONNECTIONS);
    //accept operation and connection management
    while(1) {
        csock = accept(lsock, (struct sockaddr*)&caddr, &csize);
        if(csock < 0) {
            printf("ERROR on accept operation");
            exit(EXIT_FAILURE);
        }
        //Checking space in the connection list, if not, client disconnected
        int free_space = 0;
        for(int i = 0; i < MAX_USERS; i++) {
            if(list[i].sock < 0) {
                free_space = 1;
                write(csock, WELCOME, strlen(WELCOME));
                //initializing thread and thread data space
                pthread_t               ch;     //connection handler thread space
                int                     chid;   //connection handler thread id
                chargs_t                chdata = {csock, i}; //connection handler thread data
                chid = pthread_create(&ch, NULL, _connection_handler, &chdata);
                pthread_join(ch, NULL);
                break;
            }
        }
        if(free_space == 0) {
            write(csock, NO_MORE_SPACE, strlen(NO_MORE_SPACE));
            close(csock);
        }
        free_space = 0;
    }
}