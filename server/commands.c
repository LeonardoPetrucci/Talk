//
// Created by Leonardo on 15/03/2016.
//
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "structs.h"
#include "commands.h"
#include "messages.h"
#include "macros.h"
#include "chat.h"



void close_and_cleanup(int sock, int pos, client_info* list){
    close(sock);
    memset(&list[pos], 0, sizeof(client_info));
    list[pos].sock = -1;
    pthread_exit(0);
}

void cmdManagement(int sock, int pos, client_info* list){
    int     ret;
    char    buf[MAX_MESSAGE_LENGTH];
    send(sock, WAITFORCMD, strlen(WAITFORCMD), 0);
    while (1) {
        ret = ReadSocket(sock, buf, MAX_MESSAGE_LENGTH);
        ERROR_HELPER(ret, "Error in reading from socket");

        if (strcmp(buf, QUIT) == 0) {
            ret = send(sock, QUITMESS, strlen(QUITMESS), 0);
            ERROR_HELPER(ret, "Error in sending quit_message");
            close_and_cleanup(sock, pos, list);
        }

        else if (strcmp(buf, LIST) == 0) {
            sendList(sock, list);
        }

        else if (strcmp(buf, HELP) == 0) {
            ret = send(sock, HELPMESSAGE, strlen(HELPMESSAGE), 0);
            ERROR_HELPER(ret, "Error in sending help message");
        }
        /*
        else if (strncmp(buf, CONNECTION, SIZE_CONNECTION) == 0) {

            char *username;
            strncpy(username, buf + SIZE_CONNECTION +1, strlen(buf) - SIZE_CONNECTION - 1);

            startChatSession(socket,client,username);

        }

        else {
            send(sock, WAITFORCMD, strlen(WAITFORCMD), 0);
        }*/
    }
}

void sendList(int sock, client_info* list){        //poi cambiare il tipo di ritorno per far restituire qualcosa di gestibile
    /*char* lista = "";

    for (int i = 0; i < MAX_USERS; i++){
        if (list[i].available && info->sock != list[i].sock )
            strcat(lista, clist[i].username);
            strcat(lista, ", ");
    }

    int ret = WriteSocket(socket, lista, strlen(lista));
    return ret;
     */
    int i;
    for(i = 0; i < MAX_USERS; i++) {
        if(list[i].sock != sock && list[i].available) {
            //l'assunzione è che un client disponibile ha un nome già settato, dunque non viene fatto un controllo su come è stato riempito name, che si assume correttamente impostato dal thread relativo a tale client
            send(sock, strcat(list[i].name, "\n"), strlen(list[i].name) + 1, 0);
        }
    }
}

/*int trovaPartner(char username[]){
    int i;
    for (i = 0; i < MAX_USERS; i++){
        if (strcmp(clist[i].username, username)== 0){
            return i;
        }
    }
    return -1;
}
*/
/*void startChatSession(int socket, char username[]) {

    int ret;

    //devo trovare la struct info con quello username
    client_info* altro = trovaPartner(username);

    if (altro == NULL){
        char* not_found = "Utente non trovato\n";
        ret = WriteSocket(socket,not_found,strlen(not_found));
        ERROR_HELPER(ret, "Errore in 'Utente non Trovato'");
    }

    char* found;
    sprintf(found,"Utente %s trovato! Ti mando in chat_session!\n",altro->username);
    ret = WriteSocket(socket, found,strlen(found));

    //lancio il thread che gestisce la chat session..

    pthread_t chat_thread;
    chat_args* arg = (chat_args*)malloc(sizeof(chat_args));
    arg->client1 = client;
    arg->client2 = altro;

    ret = pthread_create(&chat_thread,NULL,(void*)&chat_session,(void*)&arg);
    if (ret != 0)
        printf("Errore nella creazione del chat_thread. Code Error: %d\n", errno);

    //devo inviare segnale a thread partner
    //PROSEGUI DA QUI

    pthread_join(chat_thread,NULL);
}
*/