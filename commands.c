//
// Created by Leonardo on 15/03/2016.
//
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "server.h"
#include "structs.h"
#include "commands.h"
#include "messages.h"
#include "macros.h"
#include "chat.h"



void close_and_cleanup(int socket){
    //It should be deallcare anche le strutture dati per il client ?

    close(socket);
    //free(client_arg);
    exit(EXIT_SUCCESS);
}

void cmdManagement(int socket, client_info* client){
    while (1) {

        char buf[MAX_MESSAGE_LENGTH];
        int ret = ReadSocket(socket, buf, MAX_MESSAGE_LENGTH);
        ERROR_HELPER(ret, "Error in reading from socket");

        if (strcmp(buf, QUIT) == 0) {
            char *quit_message = "Sto chiudendo la connessione.\n";
            ret = WriteSocket(socket, quit_message, strlen(quit_message));
            ERROR_HELPER(ret, "Error in sending quit_message");

            close_and_cleanup(socket);
        }

        else if (strcmp(buf, LIST) == 0) {
            ret = sendList(socket);
            ERROR_HELPER(ret, "Error in sending user list");
        }

        else if (strcmp(buf, HELP) == 0) {
            ret = WriteSocket(socket, HELPMESSAGE, strlen(HELPMESSAGE));
            ERROR_HELPER(ret, "Error in sending help message");
        }

        else if (strncmp(buf, CONNECTION, SIZE_CONNECTION) == 0) {
            char *username;
            strncpy(username, buf + SIZE_CONNECTION +1, strlen(buf) - SIZE_CONNECTION - 1);

            //devo trovare la struct info con quello username
            client_info* altro = trovaPartner(username);

            if (*altro == NULL){
                char* not_found = "Utente non trovato";
                ret = WriteSocket(socket,not_found,strlen(not_found));
                ERROR_HELPER(ret, "Errore in 'Utente non Trovato'");
            }

            char* found;
            sprintf(found,"Utente %s trovato! Ti mando in chat_session:",altro->username);
            ret = WriteSocket(socket, found,strlen(found));

            //lancio il thread che gestisce la chat session..

            pthread_t chat_thread;
            chat_args* arg = (chat_args*)malloc(sizeof(chat_args));

            ret = pthread_create(&chat_thread,NULL,(void*)&chat_session,(void*)&arg);
            if (ret != 0)
                printf("Errore nella creazione del chat_thread. Code Error: %d", errno);

            pthread_join(chat_thread,NULL);
        }

        else {
            char *msg = "comando non riconosciuto, non faccio nulla\n";
            ret = WriteSocket(socket,msg,strlen(msg));
            ERROR_HELPER(ret,"Errore in cmdManagement");
        }
    }
}

size_t sendList(int socket, client_info* info){
    char* lista = "";

    for (int i = 0; i < MAX_USERS; i++){
        if (clist[i]->available == 1 && info->sock != clist[i].sock )
            strcat(lista, clist[i].username);
            strcat(lista, ", ");
    }

    int ret = WriteSocket(socket, lista, strlen(lista));
    return ret;

}

client_info* trovaPartner(char username[]){
    for (int i = 0; i < MAX_USERS; i++){
        if (strcmp(clist[i].username, username)== 0){
            return clist[i];
        }
    }

    return NULL;
}