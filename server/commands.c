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

void close_and_cleanup(int sock, int pos, client_info* list){
    close(sock);
    memset(&list[pos], 0, sizeof(client_info));
    list[pos].sock = -1;
    list[pos].available = -1;
}

void cmdManagement(int sock, int pos, client_info* list){
    int     ret;
    char    buf[MAX_MESSAGE_LENGTH];

    while (1) {
        memset(buf,0 ,sizeof(buf));
        ret = WriteSocket(sock, WAITFORCMD, strlen(WAITFORCMD));
        ERROR_HELPER(ret, "Error in sending WAITFORCMD");

        ret = ReadSocket(sock, buf, MAX_MESSAGE_LENGTH);

        if (errno == EAGAIN){
            ret = WriteSocket(sock,"Timeout\n",8);
            ERROR_HELPER(ret,"Error in sending Timeout");
            close_and_cleanup(sock,pos,list);
        }
        ERROR_HELPER(ret, "Error in reading from socket");

        if (list[pos].available == 0) {
            if (*buf == 'y'){

                ret = sem_signal(list[pos].sem_des,0);
                ERROR_HELPER(ret, "Error in sem_signal");

                /*
                 * sending this message i notify the client that i'm in chat session
                 */
                ret = WriteSocket(list[pos].sock,"$chat",5);
                ERROR_HELPER(ret, "Error in sending chat");

                chat_session(pos, list);

                ret = WriteSocket(list[pos].sock,"$unchat",7);
                ERROR_HELPER(ret, "Error in sending unchat");


                ERROR_HELPER(sem_wait(list[pos].list_sem,0),"Errore nella sem_wait");
                list[list[pos].partner[1]].partner[0] = -1;
                ERROR_HELPER(sem_signal(list[pos].list_sem,0),"Errore nella sem_wait");

                list[pos].partner[0] = -1;
                list[pos].partner[1] = -1;
                list[pos].available= 1;
            }

            else if (*buf =='n'){
                list[pos].available = 1;
                ret = WriteSocket(list[pos].partner[0], list[pos].name,strlen(list[pos].name));
                ERROR_HELPER(ret, "Error in sending username");

                ret = WriteSocket(list[pos].partner[0], REFUSE, strlen(REFUSE));
                ERROR_HELPER(ret, "Error in sending REFUSE message");
                list[list[pos].partner[1]].available = 1;   //MODIFICATO PERCHE QUANDO CHIAMO CONN SONO INDISPONIBILE
                list[pos].partner[0] = -1;

                ret = sem_signal(list[pos].sem_des,0);
                ERROR_HELPER(ret, "Error in sem_signal");
            }

            else{
                ret = WriteSocket(list[pos].sock,PLEASE,strlen(PLEASE));
                ERROR_HELPER(ret, "Error in sending PLEASE message");
            }
        }

        if (strcmp(buf, QUIT) == 0) {
            close_and_cleanup(sock, pos, list);
            pthread_exit(EXIT_SUCCESS);
        }

        else if (strcmp(buf, LIST) == 0) {
            ERROR_HELPER(sem_wait(list[pos].list_sem,0),"Errore nella sem_wait");
            if(sendList(sock,list) <= 0) {
                ret = WriteSocket(sock, NOBODY, strlen(NOBODY));
                ERROR_HELPER(ret, "Errore in sending nobody message");
            }
            ERROR_HELPER(sem_signal(list[pos].list_sem,0),"Errore nella sem_wait");
        }

        else if (strcmp(buf, HELP) == 0) {
            ret = WriteSocket(sock, HELPMESSAGE, strlen(HELPMESSAGE));
            ERROR_HELPER(ret, "Error in sending help message");
        }

        else if (strcmp(buf, CONNECTION) == 0) {
            //mentre cerco sono temporaneamente indisponibile
            list[pos].available = 0;
            ret = WriteSocket(sock, CHOOSE, strlen(CHOOSE));
            ERROR_HELPER(ret, "Error in sending choose message");

            int sendResult;
            ERROR_HELPER(sem_wait(list[pos].list_sem,0),"Errore nella sem_wait");
            sendResult = (sendList(sock,list));
            ERROR_HELPER(sem_signal(list[pos].list_sem,0),"Errore nella sem_signal");

            if(sendResult <= 0) {
                ret = WriteSocket(sock, NOBODY, strlen(NOBODY));
                ERROR_HELPER(ret, "Errore in sending nobody message");
                list[pos].available = 1;
                continue;
            }
            else {
                ret = WriteSocket(sock, SELECT, strlen(SELECT));
                ERROR_HELPER(ret, "Errore in sending nobody message");
            }
            ret = ReadSocket(sock, buf, strlen(buf));
            if (errno == EAGAIN){
                ret = WriteSocket(sock,"Timeout\n",8);
                ERROR_HELPER(ret,"Error in sending Timeout");
                close_and_cleanup(sock,pos,list);
            }
            if (strcmp(buf, QUIT) == 0){
                close_and_cleanup(list[pos].sock,pos,list);
                pthread_exit(EXIT_SUCCESS);
            }
            ERROR_HELPER(sem_wait(list[pos].list_sem,0),"Errore nella sem_wait");
            int found = trovaPartner(pos, buf, list);
            ERROR_HELPER(sem_signal(list[pos].list_sem,0),"Errore nella sem_signal");

            if( found >= 0) {
                //list[pos].available = 0;
                list[pos].partner[0] = list[found].sock;
                list[pos].partner[1] = found;
                ret = WriteSocket(sock, WAITCHAT, strlen(WAITCHAT));
                ERROR_HELPER(ret,"Error in sending the message");
                //SEWAIT - RACECONDITION
                ERROR_HELPER(sem_wait(list[pos].list_sem,0),"Errore nella sem_wait");
                list[found].partner[0] = list[pos].sock;
                list[found].partner[1] = pos;
                list[found].available = 0;

                ret = WriteSocket(list[found].sock, list[pos].name, strlen(list[pos].name));
                ERROR_HELPER(ret,"Error in sending the username");

                ret = WriteSocket(list[found].sock, ASKING,strlen(ASKING));
                ERROR_HELPER(ret,"Error in sending ASKING message");


                int desc = open_semaphore(list[pos].sock,1);
                ERROR_HELPER(desc, "Error in opening semaphore");

                ret = set_semaphore(desc,0,0);
                ERROR_HELPER(ret, "Errore nel setting del valore del semaforo");

                list[found].sem_des = desc;

                ret = sem_wait(desc,0);
                ERROR_HELPER(ret, "Error in sem_wait");

                ERROR_HELPER(sem_signal(list[pos].list_sem,0),"Errore nella sem_signal");
                //SEMSIGNAL - FINE RACECONDITION su FOUND

                ret = remove_semaphore(desc);
                ERROR_HELPER(ret, "fail in remove semaphore\n");

                list[found].sem_des = 0;

                if (list[found].available == 1){
                    list[pos].partner[0] = -1;
                    list[pos].partner[1] = -1;
                    list[pos].available = 1;
                }
                else {
                    /*
                     * sending this message i notify the client that i'm in chat session
                     */
                    ret = WriteSocket(list[pos].sock,"$chat",5);
                    ERROR_HELPER(ret, "Error in sending chat");

                    chat_session(pos, list);

                    ret = WriteSocket(list[pos].sock,"$unchat",7);
                    ERROR_HELPER(ret, "Error in sending unchat");
                    ret = WriteSocket(list[pos].sock, MAIN_INTRO, strlen(MAIN_INTRO));
                    ERROR_HELPER(ret, "Error in sending WELCOME");

                    ERROR_HELPER(sem_wait(list[pos].list_sem,0),"Errore nella sem_wait");
                    list[found].partner[0] = -1;
                    ERROR_HELPER(sem_signal(list[pos].list_sem,0),"Errore nella sem_signal");

                    list[pos].partner[0] = -1;
                    list[pos].partner[1] = -1;
                    list[pos].available = 1;
                }
                continue;
            }
            else {
                ret = WriteSocket(list[pos].sock, NOT_FOUND, strlen(NOT_FOUND));
                ERROR_HELPER(ret, "Error in sending notfound message");
                list[pos].available = 1;
                continue;
            }
        }

        else {
            continue;
        }
    }
}

int sendList(int sock, client_info* list){        //poi cambiare il tipo di ritorno per far restituire qualcosa di gestibile
    int somebody = 0;
    int i;
    for(i = 0; i < MAX_USERS; i++) {
        if(list[i].sock != sock && list[i].available) {
            //l'assunzione è che un client disponibile ha un nome già settato, dunque non viene fatto un controllo su come è stato riempito name, che si assume correttamente impostato dal thread relativo a tale client
            int ret = WriteSocket(sock, list[i].name, strlen(list[i].name));
            ERROR_HELPER(ret, "Errore in sending list");
            ret = WriteSocket(sock, "\n", 1);
            ERROR_HELPER(ret, "Errore in sending the new line");
            somebody++;
        }
    }
    return somebody;
}

