//
// Created by Leonardo on 15/03/2016.
//
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>

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

    while (1) {
        ret = send(sock, WAITFORCMD, strlen(WAITFORCMD), 0);
        ERROR_HELPER(ret, "Error in sending WAITFORCMD");

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

        else if (strcmp(buf, CONNECTION) == 0) {
            ret = send(sock, CHOOSE, strlen(CHOOSE),0);
            ERROR_HELPER(ret, "Error in sending choose message");
            sendList(sock,list);

            ret = ReadSocket(sock, buf, strlen(buf));
            int found = trovaPartner(buf, list);

            if( found >= 0) {
                list[pos].available = 0;
                char* connected1 = "Connected";
                ret = send(sock, connected1, strlen(connected1), 0);
                list[pos].partner = list[found].sock;

                char* connected2 = "Connected with ";
                strcat(connected2, list[pos].name);
                ret = send(list[found].sock, connected2, strlen(connected2), 0);
                list[found].partner = list[pos].sock;


            }
        }

        else {
        }

    }
}

void sendList(int sock, client_info* list){        //poi cambiare il tipo di ritorno per far restituire qualcosa di gestibile

    int i;
    for(i = 0; i < MAX_USERS; i++) {
        if(list[i].sock != sock && list[i].available) {
            //l'assunzione è che un client disponibile ha un nome già settato, dunque non viene fatto un controllo su come è stato riempito name, che si assume correttamente impostato dal thread relativo a tale client
            int ret = send(sock, list[i].name, strlen(list[i].name) , 0);
            ERROR_HELPER(ret, "Errore in sending list");
            ret = send(sock, "\n", 1, 0);
            ERROR_HELPER(ret, "Errore in sending the new line");
        }
    }
}

