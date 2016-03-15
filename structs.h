//
// Created by Leonardo on 15/03/2016.
//

#ifndef TALK_STRUCTS_H
#define TALK_STRUCTS_H

#include "macros.h"
typedef struct {
    char username[MAX_NAME_LENGTH];
    int sock;
    struct sockaddr_in address;
    int address_length;
    int available;
    int partner;
}client_info;

#endif //TALK_STRUCTS_H
