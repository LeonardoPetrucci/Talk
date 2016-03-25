//
// Created by Leonardo on 15/03/2016.
//
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>

#include "chat.h"
#include "structs.h"
#include "macros.h"
#include "messages.h"
#include "commands.h"
/*guardando il codice https://gist.github.com/silv3rm00n/5821760, la write ha una gestione più "elementare" della
 * read, che invece necessita dopo l'operazione di aggiungere il terminatore di stringa '\0' alla posizione
 * buffer[byte_letti]. Quando dobbiamo inviare le stringhe definite in messages.h facciamo la semplice write,
 * mentre quando invece dobbiamo utilizzare il buffer per inviare i messaggi, dopo la write dobbiamo fare memset
 * sul buffer per pulirlo.*/

size_t ReadSocket(int ds, char buf[], int n){
    size_t read_bytes = 0; int ret;
    while (read_bytes <= n){

        ret = recv(ds,buf + read_bytes,n,0);
        if(ret == -1 && errno == EINTR) continue;
        if(ret == 0 ) break;

        read_bytes += ret;
        if (buf[read_bytes] == '\0') break;
        n -= ret;
    }

    buf[read_bytes] = '\0';
    return read_bytes;

}


size_t WriteSocket(int ds, char buf[], int n){
    size_t written_bytes = 0; int ret;
    while(written_bytes <= n){

        ret = send(ds,buf + written_bytes,n,0);
        if (ret == -1 && errno == EINTR) continue;
        if (ret == 0) break;

        written_bytes += ret;
        n -= ret;
    }
    return written_bytes;
}

int trovaPartner(char* username, client_info* list){
    int j;
    for (j = 0; j < MAX_USERS; j++){
        if (strcmp(list[j].name, username)== 0){
            return j;
        }
    }
    return -1;
}

void chat_session(int pos, client_info* list) {

    char* ch = "\n\n- Now you're in chat! Puts '$exit' to go away.\n";
    int ret = send(list[pos].sock,ch,strlen(ch),0);
    ERROR_HELPER(ret,"Error in sending message");

    char buf[MAX_MESSAGE_LENGTH];
    char messaggio_chat[MAX_MESSAGE_CHAT];
    memset(buf,0,MAX_MESSAGE_LENGTH);
    memset(messaggio_chat,0,MAX_MESSAGE_CHAT);

    while (1){

        ret = recv(list[pos].sock,buf,MAX_MESSAGE_LENGTH,0);
        ERROR_HELPER(ret,"Errore nella read socket");

        if(list[pos].partner[0] < 0) break;

        if (strcmp(buf, "$exit") == 0) {
            ret = send(list[pos].partner[0],list[pos].name,strlen(list[pos].name),0);
            ERROR_HELPER(ret, "Error in sending the name");
            ret = send(list[pos].partner[0],END_CHAT,strlen(END_CHAT),0);
            ERROR_HELPER(ret, "Error in sending END_CHAT");
            break;
        }
        //printf("Ho ricevuto %s\n",buf);
        strcat(messaggio_chat,"[");
        strcat(messaggio_chat,list[pos].name);
        strcat(messaggio_chat, "]: ");
        strcat(messaggio_chat, buf);
        strcat(messaggio_chat, "\n");


        ret = send(list[pos].partner[0], messaggio_chat, MAX_MESSAGE_CHAT,0);
        ERROR_HELPER(ret, "Error in sending message_chat");

        memset(buf,0,MAX_MESSAGE_LENGTH);
        memset(messaggio_chat,0,MAX_MESSAGE_CHAT);

    }
}

