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

#include "macros.h"
#include "structs.h"
#include "messages.h"
#include "chat.h"
#include "commands.h"
#include "semaphore.h"

//connection handler thread
void* _connection_handler(void* args) {
    chargs_t* chargs = (chargs_t*) args;
    /*
     * TEMPORANEA?
     */
    int pos = chargs->pos;
    int sock = chargs->sock;
    client_info* list = chargs->l;

    char    buffer[MAX_MESSAGE_LENGTH]; //buffer for communication
    int     bytes;      //read or written files from the buffer
    int     check = 0;  //flag for name setting

    send(sock, WELCOME, strlen(WELCOME), 0);
    list[pos].name = (char*)malloc(MAX_NAME_LENGTH * sizeof(char));
    list[pos].sock = sock;

    while(!check) {
        send(sock, SET_NAME, strlen(SET_NAME), 0);
        bytes = ReadSocket(sock, buffer, MAX_MESSAGE_LENGTH);

        if(bytes > 0) {
            check = 1;
        }
        else {
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
            //list[chargs->pos].sock = chargs->sock;
            list[pos].partner = -1;
            list[pos].available = 1;

            break;
        }
        check = 1;
    }
    //Connection setup complete. Now this threads becomes the command listener for the specified client
    send(sock, READY, strlen(READY), 0);
    sendList(sock, list);
    cmdManagement(sock, pos, list);
}

/*void* killClient() {
    int i;
    for(i = 0; i < MAX_USERS; i++) {
        close_and_cleanup(list[i].sock, i, list);
    }
    printf("Server stopped.\n");
    exit(EXIT_SUCCESS);
}*/
