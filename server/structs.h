//
// Created by Leonardo on 17/03/2016.
//

#ifndef _STRUCTS_H
#define _STRUCTS_H
//client info, for each connected client with the server
typedef struct{
    char name[MAX_NAME_LENGTH];     //user nickname
    int sock;                       //client connection socket
    int partner;                    //partner connection socket
    int available;                  //availability flag, used for initializing a chat session
}client_info;
//conntected client list
extern client_info   list[MAX_USERS];
//connection handler thread arguments
typedef struct {
    int sock;   //connection socket
    int pos;    //client list position index
}chargs_t;
#endif //_STRUCTS_H
