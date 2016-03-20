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

//connection handler thread
void* _connection_handler(void* args) {
    //ptread_join(&pthread_self(), NULL);
    chargs_t* chargs = (chargs_t*) args;
    client_info* list = chargs->l;
    char    buffer[MAX_MESSAGE_LENGTH]; //buffer for communication
    int     bytes;      //read or written files from the buffer
    int     check = 0;  //flag for name setting
    send(chargs->sock, WELCOME, strlen(WELCOME), 0);
    list[chargs->pos].sock = chargs->sock;
    while(!check) {
        send(chargs->sock, SET_NAME, strlen(SET_NAME), 0);
        bytes = ReadSocket(chargs->sock, buffer, MAX_MESSAGE_LENGTH);
        printf("%s\n", buffer);
        //list[chargs->pos].name = (char*)calloc(bytes, sizeof(char));
        if(bytes > 0) {
            check = 1;
        }
        else {
            close(chargs->sock);
            list[chargs->pos].sock = -1;
            pthread_exit(0);
        }
        int i;
        for(i = 0; i < MAX_USERS; i++) {
            if(i != chargs->pos && list[i].sock > 0) {
                if(strcmp(buffer, list[i].name) == 0) {
                    check = 0;
                    break;
                }
            }
        }
        if(check) {
            strncpy(list[chargs->pos].name, buffer, strlen(buffer));
            list[chargs->pos].sock = chargs->sock;
            list[chargs->pos].partner = -1;
            list[chargs->pos].available = 1;
            break;
        }
    }
    //Connection setup complete. Now this threads becomes the command listener for the specified client
    send(chargs->sock, READY, strlen(READY), 0);
    sendList(chargs->sock, list);
    cmdManagement(chargs->sock, chargs->pos, chargs->l);
}

/*void* killClient() {
    int i;
    for(i = 0; i < MAX_USERS; i++) {
        close_and_cleanup(list[i].sock, i, list);
    }
    printf("Server stopped.\n");
    exit(EXIT_SUCCESS);
}*/
