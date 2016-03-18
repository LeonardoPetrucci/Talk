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

//connection handler thread
void* _connection_handler(void* args) {
    chargs_t* chargs = (chargs_t*) args;
    char    buffer[MAX_MESSAGE_LENGTH];
    while(1) {
        int check = 1;
        write(chargs->sock, SET_NAME, strlen(SET_NAME));
        for(int i = 0; i < MAX_USERS; i++) {
            //insert a well-done read that uses buffer variable!!!
            if(i != chargs->pos && list[i].sock > 0) {
                if(strncmp(buffer, list[i].name, MAX_NAME_LENGTH) == 0) {
                    check = 0;
                    break;
                }
            }
        }
        if(check == 1) {
            memcpy(list[i].name, buffer, MAX_NAME_LENGTH);
            list[i].sock = chargs->sock;
            list[i].partner = -1;
            list[i].available = 1;
            break;
        }
    }
}
