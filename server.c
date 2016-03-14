/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "server.h"

void* _connection_thread(void* param){ /*sostituisci argomenti con la struttura apposita*/
    struct connection_thread_args_t* p = (struct connection_thread_args_t*) param;

    puts("SONO IL THREAD\n");
    int read_bytes;
    char buffer[MAX_MESSAGE_LENGTH];
    char set_username[MAX_NAME_LENGTH];

    /*dobbiamo gestire con i semafori la sincronizzazione per scrivere dentro la lista dei client*/
    bzero(buffer, sizeof(buffer));
    read_bytes = write(p->sock, "Connected with the server!\n Welcome, first of all you need to create a username (max 20 characters):\n", 128);
    if(read_bytes < 0) {
        perror("ERROR on write operation");
        exit(1);
    }
    /*inserire in un ciclo while finche non viene settato correttamente il nome*/
    read_bytes = read(p->sock, buffer, 20);
    if(read_bytes < 0) {
        perror("ERROR on read operation");
        exit(1);
    }
    memcpy(&set_username, &buffer, MAX_NAME_LENGTH);
    int i; for(i = 0; i < MAX_USERS; i++) {
        if(i == p->position) continue;
        if(strcmp(p->client_list[i].username, set_username) == 0) {
            puts("This username is already used!\n");
        }
    }
    memcpy(&p->client_list[p->position].username, &set_username, MAX_NAME_LENGTH);
    puts("Username is OK.\n");
    p->client_list[p->position].available = 1;
    char* instruction = "You are in the main room. Here you can select a partner from the list below typing $connect USERNAME, where USERNAME is the chosen partner. Type $close to exit or $help to get a full command list.\n";
    read_bytes = write(p.sock, instruction, sizeof(instruction));
    for(i = 0; i < MAX_USERS; i++) {
        if(p->client_list[i].available == 1) {
            read_bytes = write(p->sock, p->client_list[i].username, sizeof(p->client_list[i].username));
            if(read_bytes < 0){
                perror("ERROR on write operation\n");
                exit(1);
            }
        }
    }

}

int main(int argc, char *argv[]) {
    /*  Occorre passare al main l'indirizzo IP e la porta su cui si vuole
     *  far girare il server.
     */
    if(argc < 3) {
        puts("Please insert a IP address and a port number\n");
        exit(1);
    }
    /*  Inizializzazione della variabili che servono al server:
     *  - Descrittore socket
     *  - Indirizzo IP del server
     *  - Numero di porta
     *  - Lista dei client connessi
     *  - Buffer per messaggi
     *    - Username del client
     *    - Socket del clent
     *    - Indirizzo del client
     *    - Lunghezza indirizzo client
     *    - Flag di disponibilità
     */
    int server_socket, client_socket, client_length, port_number = atoi(argv[2]);
    struct sockaddr_in server_address, client_address;
    struct client_info_t connected_clients[MAX_USERS];
    char*ip_address = argv[1];

    bzero((char*)&server_address, sizeof(server_address));
    bzero((char*)&connected_clients, sizeof(connected_clients));
    int i;for(i = 0; i < MAX_USERS; i++) {
        connected_clients[i].connection_socket = -2;
    }
    
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_socket < 0) {
        perror("ERROR on socket operation");
        exit(1);
    }
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    server_address.sin_port = htons(port_number);
    
    if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("ERROR on bind operation");
        exit(1);
    }
    
    printf("Server running...\nAddress: %s Port number: %d\n", ip_address, port_number);
    listen(server_socket, 5);
    
    while(1) {
        for(i = 0; i < MAX_USERS; i++) {
            if(connected_clients[i].connection_socket < -1) {
                connected_clients[i].address_length = sizeof(connected_clients[i].address);
                connected_clients[i].connection_socket = accept(server_socket, (struct sockaddr*)&connected_clients[i].address, &connected_clients[i].address_length);
                if(connected_clients[i].connection_socket < 0) {
                    perror("ERROR on accept operation");
                    exit(1);
                }
                int handler;
                pthread_t thread_handler;
                struct connection_thread_args_t args;
                bzero((char*)&args, sizeof(args));
                args.sock = connected_clients[i].connection_socket;
                args.client_list = connected_clients;
                args.position = i;
                handler = pthread_create(&handler, NULL, _connection_thread, (void*)&args);
                assert(handler == 0);
                pthread_join(thread_handler, NULL);
            }
        }

    }
}