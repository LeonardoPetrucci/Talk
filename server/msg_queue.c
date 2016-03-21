#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

#include "commands.h"
#include "chat.h"
#include "structs.h"

int initialize_queue(int key, int flag){ //se 0 la creo e la apro, se 1 la apro soltanto
    int ret;
    if(!flag) {
        ret = msgget(key, IPC_CREAT|0660);
        ERROR_HELPER(ret, "cannot initializse queue\n");
        return ret;
    }
    ret = msgget(key, IPC_CREAT);
    ERROR_HELPER(ret, "cannot initializse queue\n");
    return ret;
}

int enqueue(char* nickname, char* msg, int queue) {
    msg_t message;
    strcpy(message.mittente,  nickname);
    strcpy(message.messaggio, msg);
    int ret = msgsnd(queue, &message, 0, sizeof(message));
    ERROR_HELPER(ret, "cannot send in queue\n");
    return ret;
}

msg_t* dequeue(int queue, char* dest) {
    msg_t message;


}
/*
 * altro metodo utilizzare msgget che crea una coda di messaggi.
 */