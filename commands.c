//
// Created by Leonardo on 15/03/2016.
//
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"
#include "commands.h"
#include "messages.h"
#include "macros.h"



void close_and_cleanup(int socket){
    //It should be deallcare anche le strutture dati per il client ?

    close(socket);
    //free(client_arg);
    exit(EXIT_SUCCESS);
}

void cmdManagement(int socket){
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
            char *user_name;
            strncpy(user_name, buf + SIZE_CONNECTION, strlen(buf) - SIZE_CONNECTION);
            //settaggio chat_session ...


        }

        else {
            char *msg = "comando non riconosciuto, non faccio nulla\n";
            ret = WriteSocket(socket,msg,strlen(msg));
            ERROR_HELPER(ret,"Errore in cmdManagement");
        }
    }
}

size_t sendList(int socket){

}