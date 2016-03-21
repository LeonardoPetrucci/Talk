//
// Created by Leonardo on 17/03/2016.
//

#ifndef _STRUCTS_H
#define _STRUCTS_H
//client info, for each connected client with the server
typedef struct{
    char*           name;                       //user nickname
    int             sock;                       //client connection socket
    int             partner;                    //partner connection socket
    int             available;                  //availability flag, used for initializing a chat session
    pthread_t       chandler;                         //connection handler thread
    int             msg_des;
}client_info;
//conntected client list
//extern              client_info *list; //*list;
//connection handler thread arguments
typedef struct {
    client_info*    l;
    int             sock;   //connection socket
    int             pos;    //client list position index
}chargs_t;

typedef struct {
    char* mittente;
    long type;
    char* messaggio;
}msg_t;
#endif //_STRUCTS_H
