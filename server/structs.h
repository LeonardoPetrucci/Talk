//
// Created by Leonardo on 17/03/2016.
//

#ifndef _STRUCTS_H
#define _STRUCTS_H

#include "macros.h"
//client info, for each connected client with the server
typedef struct{
    char*           name;                       //user nickname
    int             sock;                       //client connection socket
    int             partner[2];                    //partner socket in [0] e position in list[1]
    int             available;                  //availability flag, used for initializing a chat session
    pthread_t       chandler;                         //connection handler thread
    int             sem_des;
    int             list_sem;                   //desc on list semaphore
}client_info;
//conntected client list
extern              client_info *list; //*list;
//extern              int list_sem;
//connection handler thread arguments
typedef struct {
    client_info*    l;
    int             sock;   //connection socket
    int             pos;    //client list position index
}chargs_t;

typedef struct {
    char           mittente[MAX_NAME_LENGTH];
    int            destinatario;
    long           type;
    char           messaggio[MAX_MESSAGE_LENGTH];
}msg_t;
#endif //_STRUCTS_H
