#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "macros.h"
#include "structs.h"
#include "messages.h"
#include "chat.h"

//connection handler thread
void* _connection_handler(void* args) {
    chargs_t* chargs = (chargs_t*) args;
    client_info* list = chargs->l;
    char    buffer[MAX_MESSAGE_LENGTH]; //buffer for communication
    char    name[MAX_NAME_LENGTH];
    int     bytes;      //read or written files from the buffer
    int     check = 0;  //flag for name setting
    write(chargs->sock, WELCOME, strlen(WELCOME));
    list[chargs->pos].sock = chargs->sock;
    printf("Connected client at position: %d\n", chargs->pos);
    while(!check) {
        write(chargs->sock, SET_NAME, strlen(SET_NAME));
        printf("Waiting for a username...\n");
        bytes = ReadSocket(chargs->sock, buffer, MAX_MESSAGE_LENGTH);
        if(bytes > 0) {
            printf("Chosen name: %s\n", buffer);
            check = 1;
        }
        else {
            printf("Client disconnected.\n");
            close(chargs->sock);
            list[chargs->pos].sock = -1;
            pthread_exit(0);
        }
        for(int i = 0; i < MAX_USERS; i++) {
            printf("%d\n", i);
            if(i != chargs->pos && list[i].sock > 0) {
                if(strncmp(buffer, list[i].name, MAX_NAME_LENGTH) == 0) {
                    check = 0;
                    printf("Username fault. ");
                    break;
                }
            }
        }
        printf("for comoleted, %d\n", check);
        if(check) {
            printf("Check value: %d\n", check);
            strncpy(list[chargs->pos].name, buffer, strlen(buffer)); //COLPA TUA
            printf("non è colpa mia memcpy");
            list[chargs->pos].sock = chargs->sock;
            printf("non è colpa mia sock");
            list[chargs->pos].partner = -1;
            printf("non è colpa mia partner");
            list[chargs->pos].available = 1;
            printf("non è colpa mia available");
            printf("Setting username... DONE. Redirecting client in the main room...");
            break;
        }
    }
    printf("sono uscito\n");
    pthread_exit(0);
}


