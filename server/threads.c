#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "macros.h"
#include "structs.h"
#include "messages.h"
#include "chat.h"
#include "commands.h"
#include "semaphore.h"

client_info* list;
int list_sem;

//connection handler thread
void* _connection_handler(void* args) {
    chargs_t* chargs = (chargs_t*) args;
    int pos = chargs->pos;
    int sock = chargs->sock;
    list = chargs->l;

    char    buffer[MAX_MESSAGE_LENGTH] = {0}; //buffer for communication
    int     bytes;      //read or written files from the buffer
    int     check = 0;  //flag for name setting

    int ret = WriteSocket(sock,WELCOME,strlen(WELCOME));
    ERROR_HELPER(ret, "Error in sending WELCOME");

    //Timeout connection:client doesn't send anything for 5 minutes.
    struct timeval tv;
    tv.tv_sec = 60*5;
    tv.tv_usec = 0;
    ret = setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(tv));
    ERROR_HELPER(ret, "Error in setsockopt");


    list[pos].name = (char*)calloc(sizeof(char), MAX_NAME_LENGTH);
    list[pos].sock = sock;
    //setto questa variabile globale, che indica il descrittore del sem sulla lista, in modo che sia visibile da killClient e killServer
    list_sem = list[pos].list_sem;

    while(!check) {
        ret = WriteSocket(sock, SET_NAME, strlen(SET_NAME));
        ERROR_HELPER(ret, "Error in sending SET_NAME");

        bytes = ReadSocket(sock, buffer, MAX_MESSAGE_LENGTH);
        if (errno == EAGAIN){ //if errno = EAGAIN it means that client haven't send anything to server in 5 minutes.
            ret = WriteSocket(sock,"Timeout\n",8);
            ERROR_HELPER(ret,"Error in sending Timeout");
            close_and_cleanup(sock,pos,list);
        }
        //if buffer è ctrl c invia quit

        if(bytes > 0) {
            check = 1;
        }
        else {
            close(sock);
            list[pos].sock = -1;
            pthread_exit(0);
        }

        if (strcmp(buffer,"$quit") == 0){
            close(sock);
            list[pos].sock = -1;
            pthread_exit(0);
        }

        int i;
        for(i = 0; i < MAX_USERS; i++) {
            if(i != pos && list[i].sock > 0) {
                if(strcmp(buffer, list[i].name) == 0) {
                    check = 0;
                    break;
                }
            }
        }
        if(check) {
            strncpy(list[pos].name, buffer, strlen(buffer));
            list[pos].partner[0] = -1;
            list[pos].partner[1] = -1;
            list[pos].available = 1;

            break;
        }
        memset(&buffer, 0, sizeof(buffer)); //controlla funzionamento
        check = 0;
    }
    //Connection setup complete. Now this threads becomes the command listener for the specified client
    ret = WriteSocket(sock, READY, strlen(READY));
    ERROR_HELPER(ret,"Error in sending READY");

    //RACE CONDITION
    ERROR_HELPER(sem_wait(list[pos].list_sem,0),"Errore nella sem_wait,lista");

    if(sendList(sock, list) <= 0) {
        ret = WriteSocket(sock, NOBODY, strlen(NOBODY));
        ERROR_HELPER(ret,"Error in sending NOBODY");
    }
    ERROR_HELPER(sem_signal(list[pos].list_sem,0),"Errore nella sem_wait");
    //Fine RACE CONDITION

    cmdManagement(sock, pos, list);
}

void killClient() {
    int i,ret;
    for(i = 0; i < MAX_USERS; i++) {
        if (list[i].sock != -1) {
            ret = WriteSocket(list[i].sock, KILL_CLIENT, strlen(KILL_CLIENT));
            ERROR_HELPER(ret, "Error in sending KILL_CLIENT message");

            ret = remove_semaphore(list_sem);
            ERROR_HELPER(ret, "error in remove semaphore");

            close_and_cleanup(list[i].sock, i, list);
        }
    }
    printf("Server stopped.\n");
    exit(EXIT_SUCCESS);
}

void killServer() {
    free(list);

    //int ret = remove_semaphore(list_sem);
    //ERROR_HELPER(ret, "error in remove semaphore");

    printf("\nServer stopped");
    exit(EXIT_SUCCESS);
}