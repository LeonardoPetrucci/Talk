#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "structs.h"
#include "commands.h"
#include "messages.h"
#include "semaphore.h"
#include "chat.h"

client_info* list;

void close_and_cleanup(int sock, int pos, client_info* list){
    close(sock);
    memset(&list[pos], 0, sizeof(client_info));
    list[pos].sock = -1;
}

void cmdManagement(int sock, int pos, client_info* list){
    int     ret;
    char    buf[MAX_MESSAGE_LENGTH];

    while (1) {
        memset(buf,0 ,sizeof(buf));
        ret = WriteSocket(sock, WAITFORCMD, strlen(WAITFORCMD));
        ERROR_HELPER(ret, "ERROR - commands.c line 34");

        ret = ReadSocket(sock, buf, MAX_MESSAGE_LENGTH);
        if (errno == EAGAIN){
            ret = WriteSocket(sock,"Timeout\n",8);
            ERROR_HELPER(ret,"ERROR - commands.c line 39");
            close_and_cleanup(sock,pos,list);
        }
        if(errno == ECONNRESET) {
            close_and_cleanup(list[pos].sock,pos,list);
            pthread_exit(0);
        }
        ERROR_HELPER(ret, "ERROR - commands.c line 46");

        if (list[pos].available == 0) {
            if (*buf == 'y') {
                ret = sem_signal(list[pos].sem_des,0);
                ERROR_HELPER(ret, "ERROR - commands.c line 51");
                /*
                 * sending this message i notify the client that i'm in chat session
                 */
                ret = WriteSocket(list[pos].sock,"$chat",5);
                ERROR_HELPER(ret, "ERROR - commands.c line 56");

                chat_session(pos, list);

                ret = WriteSocket(list[pos].sock,"$unchat",7);
                if(errno == ECONNRESET) {
                    close_and_cleanup(list[pos].sock,pos,list);
                    pthread_exit(0);
                }
                ERROR_HELPER(ret, "ERROR - commands.c line 65");

                ret = WriteSocket(list[pos].sock, MAIN_INTRO, strlen(MAIN_INTRO));
                ERROR_HELPER(ret, "ERROR - commands.c line 68");

                ERROR_HELPER(sem_wait(list[pos].list_sem,0),"ERROR - commands.c line 70");
                list[list[pos].partner[1]].partner[0] = -1;
                ERROR_HELPER(sem_signal(list[pos].list_sem,0),"ERROR - commands.c line 72");

                list[pos].partner[0] = -1;
                list[pos].partner[1] = -1;
                list[pos].available= 1;
            }
            //AGGIUNTA OR STRCMP NELL'ELSE IF
            else if (*buf =='n' || strcmp(buf, QUIT) == 0){
                list[pos].available = 1;
                ret = WriteSocket(list[pos].partner[0], list[pos].name,strlen(list[pos].name));
                ERROR_HELPER(ret, "ERROR - commands.c line 82");

                ret = WriteSocket(list[pos].partner[0], REFUSE, strlen(REFUSE));
                ERROR_HELPER(ret, "ERROR - commands.c line 85");
                list[list[pos].partner[1]].available = 1;   //MODIFICATO PERCHE QUANDO CHIAMO CONN SONO INDISPONIBILE
                list[pos].partner[0] = -1;

                ret = sem_signal(list[pos].sem_des,0);
                ERROR_HELPER(ret, "Error in sem_signal");
                if(strcmp(buf, QUIT) == 0) {
                    close_and_cleanup(sock, pos, list);
                    pthread_exit(0);
                }
            }
            else{
                ret = WriteSocket(list[pos].sock,PLEASE,strlen(PLEASE));
                ERROR_HELPER(ret, "ERROR - commands.c line 90");
            }
        }

        if (strcmp(buf, QUIT) == 0) {
            close_and_cleanup(sock, pos, list);
            pthread_exit(0);
        }

        else if (strcmp(buf, LIST) == 0) {
            ERROR_HELPER(sem_wait(list[pos].list_sem,0),"ERROR - commands.c line 105");
            if(sendList(sock,list) <= 0) {
                ret = WriteSocket(sock, NOBODY, strlen(NOBODY));
                ERROR_HELPER(ret, "ERROR - commands.c line 108");
            }
            ERROR_HELPER(sem_signal(list[pos].list_sem,0),"ERROR - commands.c line 110");
        }

        else if (strcmp(buf, HELP) == 0) {
            ret = WriteSocket(sock, HELPMESSAGE, strlen(HELPMESSAGE));
            ERROR_HELPER(ret, "ERROR - commands.c line 115");
        }

        else if (strcmp(buf, CONNECTION) == 0) {
            //mentre cerco sono temporaneamente indisponibile
            list[pos].available = 0;
            ret = WriteSocket(sock, CHOOSE, strlen(CHOOSE));
            ERROR_HELPER(ret, "ERROR - commands.c line 122");

            int sendResult;
            ERROR_HELPER(sem_wait(list[pos].list_sem,0),"ERROR - commands.c line 125");
            sendResult = (sendList(sock,list));
            ERROR_HELPER(sem_signal(list[pos].list_sem,0),"ERROR - commands.c line 127");

            if(sendResult <= 0) {
                ret = WriteSocket(sock, NOBODY, strlen(NOBODY));
                ERROR_HELPER(ret, "ERROR - commands.c line 131");
                list[pos].available = 1;
                continue;
            }
            else {
                ret = WriteSocket(sock, SELECT, strlen(SELECT));
                ERROR_HELPER(ret, "ERROR - commands.c line 137");
            }
            ret = ReadSocket(sock, buf, strlen(buf));
            if (errno == EAGAIN){
                ret = WriteSocket(sock,"Timeout\n",8);
                ERROR_HELPER(ret,"ERROR - commands.c line 142");
                close_and_cleanup(sock,pos,list);
            }
            if (strcmp(buf, QUIT) == 0){
                close_and_cleanup(list[pos].sock,pos,list);
                pthread_exit(0);
            }
            ERROR_HELPER(sem_wait(list[pos].list_sem,0),"ERROR - commands.c line 149");
            int found = trovaPartner(pos, buf, list);
            ERROR_HELPER(sem_signal(list[pos].list_sem,0),"ERROR - commands.c line 151");

            if( found >= 0) {
                //list[pos].available = 0;
                list[pos].partner[0] = list[found].sock;
                list[pos].partner[1] = found;
                ret = WriteSocket(sock, WAITCHAT, strlen(WAITCHAT));
                ERROR_HELPER(ret,"ERROR - commands.c line 158");
                //SEWAIT - RACECONDITION
                ERROR_HELPER(sem_wait(list[pos].list_sem,0),"ERROR - commands.c line 160");
                list[found].partner[0] = list[pos].sock;
                list[found].partner[1] = pos;
                list[found].available = 0;

                ret = WriteSocket(list[found].sock, list[pos].name, strlen(list[pos].name));
                ERROR_HELPER(ret,"ERROR - commands.c line 166");

                ret = WriteSocket(list[found].sock, ASKING,strlen(ASKING));
                ERROR_HELPER(ret,"ERROR - commands.c line 169");


                int desc = open_semaphore(list[pos].sock,1);
                ERROR_HELPER(desc, "ERROR - commands.c line 174");

                ret = set_semaphore(desc,0,0);
                ERROR_HELPER(ret, "ERROR - commands.c line 176");

                list[found].sem_des = desc;

                ret = sem_wait(desc,0);
                ERROR_HELPER(ret, "ERROR - commands.c line 183");

                ERROR_HELPER(sem_signal(list[pos].list_sem,0),"ERROR - commands.c line 185");
                //SEMSIGNAL - FINE RACECONDITION su FOUND

                ret = remove_semaphore(desc);
                ERROR_HELPER(ret, "ERROR - commands.c line 189\n");

                list[found].sem_des = 0;

                if (list[found].available == 1 && list[found].sock != -1){
                    list[pos].partner[0] = -1;
                    list[pos].partner[1] = -1;
                    list[pos].available = 1;
                }
                else {
                    /*
                     * sending this message i notify the client that i'm in chat session
                     */
                    ret = WriteSocket(list[pos].sock,"$chat",5);
                    if (errno == ECONNRESET){
                        close_and_cleanup(list[pos].sock,pos,list);
                        pthread_exit(0);
                    }
                    ERROR_HELPER(ret, "Error in sending chat");

                    chat_session(pos, list);

                    ret = WriteSocket(list[pos].sock,"$unchat",7);
                    if(errno == ECONNRESET) {
                        close_and_cleanup(list[pos].sock,pos,list);
                        pthread_exit(0);
                    }
                    ERROR_HELPER(ret, "ERROR - commands.c line 215");

                    ret = WriteSocket(list[pos].sock, MAIN_INTRO, strlen(MAIN_INTRO));
                    ERROR_HELPER(ret, "ERROR - commands.c line 218");

                    ERROR_HELPER(sem_wait(list[pos].list_sem,0),"ERROR - commands.c line 220");
                    list[found].partner[0] = -1;
                    ERROR_HELPER(sem_signal(list[pos].list_sem,0),"ERROR - commands.c line 222");

                    list[pos].partner[0] = -1;
                    list[pos].partner[1] = -1;
                    list[pos].available = 1;
                }
                continue;
            }
            else {
                ret = WriteSocket(list[pos].sock, NOT_FOUND, strlen(NOT_FOUND));
                ERROR_HELPER(ret, "ERROR - commands.c line 232");
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
            ERROR_HELPER(ret, "ERROR - commands.c line 251");
            ret = WriteSocket(sock, "\n", 1);
            ERROR_HELPER(ret, "ERROR - commands.c line 253");
            somebody++;
        }
    }
    return somebody;
}


