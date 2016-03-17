//
// Created by Leonardo on 15/03/2016.
//

#ifndef TALK_COMMANDS_H
#define TALK_COMMANDS_H

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

/*
 * Manage special command between client and server
 */
void cmdManagement(int socket, client_info* client);
/*
 * It sends utents-connected-and-avalaible list. It returns the number of bytes sent.
 */
size_t  sendList(int socket, client_info* info);
/*
 * Manage closure and cleanup of resources
 */
void close_and_cleanup(int socket);
/*
 * Funzione ausiliaria che trova dato lo username il partner per una chat_session
 */
client_info* trovaPartner(char username[]);
/*
 * Esegue tutte le operazioni preliminari per l'inizio di una chat session
 */
void startChatSession(int socket, client_info* client, char username[]);

#endif //TALK_COMMANDS_H
