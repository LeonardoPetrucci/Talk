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
    client_info            list[MAX_USERS];//= (client_info*) malloc(MAX_USERS*sizeof(client_info));    //try with dynamic allocation
    printf("Initializing connected client list... DONE, size: %d bytes\n", sizeof(list));
    //declaration of server elements
    int                     lsock;  //listening socket
    int                     csock;  //connection socket
    struct sockaddr_in      saddr;  //server address
    struct sockaddr_in      caddr;  //client address
    int                     csize;  //client address size
    printf("Declaration of server elements... DONE\n");
    //BUILDING SERVER CONNECTION STRUCTURE
    //socket operation
    if(((lsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) < 0) {
        printf("ERROR on socket operation.\n");
        exit(EXIT_FAILURE);
    }
    printf("Socket operation... DONE\n");
    //Initializing connected client list
    memset(&list, 0, sizeof(list));
    int i;
    for(i = 0; i < MAX_USERS; i++) {
        list[i].sock = -1;
    }
    printf("Setting up connected client list... DONE\n");
    //Initializing server elements
    memset(&saddr, 0, sizeof(saddr));
    memset(&caddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    if(argc > 1) saddr.sin_port = htons(atoi(argv[1])); else saddr.sin_port = htons(DEFAULT_PORT);
    printf("Inizializing server elements... DONE\n");
    //bind operation
    if(bind(lsock, (struct sockaddr*)&saddr, sizeof(saddr)) < 0){
        printf("ERROR on bind operation.\n");
        exit(EXIT_FAILURE);
    }
    printf("Bind operation... DONE\n");
    //listen operation
    listen(lsock, MAX_CONNECTIONS);
    printf("Listen operation... DONE\n");
    //accept operation and connection management
    while(1) {
        csock = accept(lsock, (struct sockaddr*)&caddr, &csize);
        if(csock < 0) {
            printf("ERROR on accept operation");
            exit(EXIT_FAILURE);
        }
        printf("Accept operation... DONE\n");
        //Checking space in the connection list, if not, client disconnected
        int free_space = 0;
        int i;
        for(i = 0; i < MAX_USERS; i++) {
            if(list[i].sock < 0) {
                free_space = 1;
                //initializing thread and thread data space
                pthread_t               ch;     //connection handler thread space
                int                     chid;   //connection handler thread id
                chargs_t                chdata = {list, csock, i}; //connection handler thread data
                chid = pthread_create(&ch, NULL, _connection_handler, &chdata);
                printf("Initializing connection handler thread for client number %d... DONE\n", i);
                pthread_join(ch, NULL);
                break;
            }
        }
        if(free_space == 0) {
            write(csock, NO_MORE_SPACE, strlen(NO_MORE_SPACE));
            close(csock);
            printf("No more space for this client, closing socket... DONE");
        }
        free_space = 0;
    }
}