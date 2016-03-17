//
// Created by Leonardo on 15/03/2016.
//

#ifndef TALK_STRUCTS_H
#define TALK_STRUCTS_H

#include <netinet/in.h>
#include "macros.h"


typedef struct {
    char username[MAX_NAME_LENGTH];
    int sock;
    int available;
    int partner;
    int position; //per evitare di fare cicli for ogni volta che si vuole interagire con il client, da vedere meglio
} client_info;

client_info clist[MAX_USERS];

typedef struct msg_s {
    char mittente[MAX_NAME_LENGTH];
    long type;
    char msg[MAX_MESSAGE_LENGTH];
} msg_t;

typedef struct chat_arg{
    client_info* client1;
    client_info* client2;
} chat_args;

#endif //TALK_STRUCTS_H
