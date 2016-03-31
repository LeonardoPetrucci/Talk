//CAUTION!!! THIS CODE IS WRITTEN IN C99, TO COMPILE IT WITH COMMAND LINE, YOU NEED TO USE -std=c99
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/wait.h>

#include "macros.h"
#include "structs.h"
#include "messages.h"
#include "threads.h"
#include "semaphore.h"
#include "commands.h"

int main(char argc, char* argv[]){

    signal(SIGINT,killServer);
    signal(SIGHUP,killServer);
    signal(SIGQUIT,killServer);
    signal(SIGKILL,killServer);
    signal(SIGTERM,killServer);
    signal(SIGILL, killServer);

    //declaration of server elements
    client_info*            list;
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
    list = (client_info*)calloc(MAX_USERS, sizeof(client_info));

    int ret_sem = open_semaphore(20,1);
    ERROR_HELPER(ret_sem,"Error in opening list_sem");

    ERROR_HELPER(set_semaphore(ret_sem,0,1),"Errore nel setting di list_sem");
    //Initializing connected client list
    int i;
    for(i = 0; i < MAX_USERS; i++) {
        list[i].name = (char*)malloc(MAX_NAME_LENGTH* sizeof(char));
        list[i].sock = -1;

    }
    //Initializing server elements
    memset(&saddr, 0, sizeof(saddr));
    memset(&caddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    if(argc > 2) saddr.sin_port = htons(atoi(argv[1])); else saddr.sin_port = htons(DEFAULT_PORT);
    //bind operation
    if(bind(lsock, (struct sockaddr*)&saddr, sizeof(saddr)) < 0){
        printf("ERROR on bind operation.\n");
        exit(EXIT_FAILURE);
    }
    //executing ifconfig operation using a child process
    pid_t ifconfig;
    int status;
    ifconfig = fork();
    if(ifconfig == -1) {
        printf("Error: cannot fork.\n");
        exit(EXIT_FAILURE);
    }
    if(ifconfig == 0) {
        //chiamo la exec su ifconfig
        execl("/sbin/ifconfig", "ifconfig", 0);
        printf("ERROR: cannot complete ifconfig operation.\n");
    }
    else {
        wait(&status);
    }
        //listen operation
        listen(lsock, MAX_CONNECTIONS);
        printf("Server running...\n");

        //accept operation and connection management
    int free_space = 0;
        while (1) {
            csock = accept(lsock, (struct sockaddr *) &caddr, &csize);
            if (csock < 0) {
                ERROR_HELPER(csock, "Errore nella accept");
            }
            //Checking space in the connection list, if not, client disconnected

            int i;
            for (i = 0; i < MAX_USERS; i++) {
                if (list[i].sock < 0) {
                    free_space = 1;
                    list[i].list_sem = ret_sem;
                    //initializing thread and thread data space
                    //pthread_t               ch;     //connection handler thread space
                    chargs_t chdata = {list, csock, i}; //connection handler thread data
                    pthread_create(&list[i].chandler, NULL, _connection_handler, &chdata);
                    signal(SIGINT, killClient);
                    signal(SIGHUP, killClient);
                    signal(SIGQUIT, killClient);
                    signal(SIGKILL, killClient);
                    signal(SIGTERM, killClient);
                    signal(SIGILL, killClient);
                    signal(SIGPIPE, PipeHandler);

                    pthread_detach(list[i].chandler);
                    break;
                }
            }
            if (free_space == 0) {
                write(csock, NO_MORE_SPACE, strlen(NO_MORE_SPACE));
                close(csock);
            }
            free_space = 0;
        }

}