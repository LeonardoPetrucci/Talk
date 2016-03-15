
#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include "structs.h"

#define MAX_MESSAGE_LENGTH  256
#define MAX_USERS           20
#define MAX_NAME_LENGTH     20
#define LIST "$list"
#define QUIT "$quit"
#define HELP "$help"
#define QUIT_CONNECTION "$exit"

#define CONNECTION "$conn"
#define SIZE_CONNECTION 5

#define HELPMESSAGE "Puoi mandare questi comandi:\n$list - invia la lista degli utenti disponibili per una conversazione\n$quit - esce dalla chat\n$connection USERNAME - inizia una chat session con l'utente USERNAME se possibile\n$quit_connection - permette di uscire da una chat session\n"

#define ERROR_HELPER(ret, message) do{                              \
    if (ret < 0){                                                   \
        fprintf(stderr, "%s: %s\n", message, strerror(errno));      \
        exit(EXIT_FAILURE);                                         \
    }                                                               \
} while(0)

client_info clist[MAX_USERS]; //massimo numero di utenti nella chatroom


#endif /* SERVER_H */

