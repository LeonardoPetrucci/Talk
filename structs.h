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
    struct sockaddr_in address;
    int address_length;
    int available;
    int partner;
} client_info;


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
