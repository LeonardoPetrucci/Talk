//
// Created by Leonardo on 15/03/2016.
//

#ifndef TALK_COMMANDS_H
#define TALK_COMMANDS_H

#define LIST            "$list"
#define QUIT            "$quit"
#define HELP            "$help"
#define CONNECTION      "$conn"

#define ERROR_HELPER(ret, message) do {                                 \
if (ret < 0) {                                                          \
    fprintf(stderr, "%s: %s\n", message, strerror(errno));              \
    exit(EXIT_FAILURE);                                                 \
    }                                                                   \
}while(0)

void    cmdManagement(int socket);
size_t  sendList(int socket);
void    close_and_cleanup(int socket);


#endif //TALK_COMMANDS_H
