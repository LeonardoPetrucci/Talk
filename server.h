/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   server.h
 * Author: leonardo
 *
 * Created on 8 marzo 2016, 10.10
 */

#ifndef SERVER_H
#define SERVER_H

#define MAX_MESSAGE_LENGTH  256
#define MAX_USERS           20
#define MAX_NAME_LENGTH     20

typedef struct client_info_t {
    char* username;
    int connection_socket;
    struct sockaddr_in address;
    int address_length;
    int available;
}client_info_s;

typedef struct connection_thread_args_t {
    int sock;
    struct client_info_t* client_list;
    int position;
}connection_thread_args_s;

#endif /* SERVER_H */

