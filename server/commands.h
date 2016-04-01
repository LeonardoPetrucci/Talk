
#ifndef TALK_COMMANDS_H
#define TALK_COMMANDS_H

#include "structs.h"

#define LIST            "$list"
#define QUIT            "$quit"
#define HELP            "$help"
#define CONNECTION      "$conn"

#define ERROR_HELPER(ret, message) do{                              \
    if (ret < 0){                                                   \
        fprintf(stderr, "%s: %s\n", message, strerror(errno));      \
        exit(EXIT_FAILURE);                                         \
    }                                                               \
} while(0)

 // Manage special command between client and server

void cmdManagement(int sock, int pos, client_info* list);

 // It sends utents-connected-and-avalaible list. It returns the number of bytes sent.

int sendList(int sock, client_info* list);  //ho eliminato il parametro lista, tanto è extern e la vede comunque

 // Manage closure and cleanup of resources

void close_and_cleanup(int sock, int pos, client_info* list);


#endif //TALK_COMMANDS_H
