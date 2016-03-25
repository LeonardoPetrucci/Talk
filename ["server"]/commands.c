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
#include <signal.h>
#include <sys/msg.h>

#include "structs.h"
#include "commands.h"
#include "messages.h"
#include "macros.h"
#include "semaphore.h"
#include "chat.h"

int queue_key = 30;


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

        if (list[pos].available == 0) {
            if (*buf == 'y'){

                ret = sem_signal(list[pos].sem_des,0);
                ERROR_HELPER(ret, "Error in sem_signal");

                chat_session(pos, list);

                list[list[pos].partner[1]].partner[0] = -1;
                list[pos].partner[0] = -1;
                list[pos].partner[1] = -1;
                list[pos].available= 1;
            }

            else if (*buf =='n'){
                list[pos].available = 1;
                ret = send(list[pos].partner[0], list[pos].name,strlen(list[pos].name),0);
                ERROR_HELPER(ret, "Error in sending username");

                ret = send(list[pos].partner[0], REFUSE, strlen(REFUSE),0);
                ERROR_HELPER(ret, "Error in sending REFUSE message");

                list[pos].partner[0] = -1;

                ret = sem_signal(list[pos].sem_des,0);
                ERROR_HELPER(ret, "Error in sem_signal");
            }

            else{
                ret = send(list[pos].sock,PLEASE,strlen(PLEASE),0);
                ERROR_HELPER(ret, "Error in sending PLEASE message");
            }
        }

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
                list[pos].partner[0] = list[found].sock;
                list[pos].partner[1] = found;

                //SEWAIT - RACECONDITION

                list[found].partner[0] = list[pos].sock;
                list[found].partner[1] = pos;
                list[found].available = 0;


                ret = send(list[found].sock, list[pos].name, strlen(list[pos].name), 0);
                ERROR_HELPER(ret,"Error in sending the username");

                ret = send(list[found].sock, ASKING,strlen(ASKING),0);
                ERROR_HELPER(ret,"Error in sending ASKING message");


                //SEMSIGNAL

                int desc = open_semaphore(list[pos].sock,1);
                ERROR_HELPER(desc, "Error in opening semaphore");

                ret = set_semaphore(desc,0,0);
                ERROR_HELPER(ret, "Errore nel setting del valore del semaforo");

                list[found].sem_des = desc;

                ret = sem_wait(desc,0);
                ERROR_HELPER(ret, "Error in sem_wait");

                ret = remove_semaphore(desc);
                ERROR_HELPER(ret, "fail in remove semaphore\n");

                list[found].sem_des = 0;

                if (list[found].available == 1){
                    list[pos].partner[0] = -1;
                    list[pos].partner[1] = -1;
                    list[pos].available = 1;
                }
                else {
                    chat_session(pos, list);

                    list[found].partner[0] = -1;
                    list[pos].partner[0] = -1;
                    list[pos].partner[1] = -1;
                    list[pos].available = 1;
                }
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

