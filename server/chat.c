#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>

#include "chat.h"
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
    while (n > 0) {
        ret = send(ds, buf + written_bytes, n, 0);
        if (ret == -1)
            if (errno == EINTR)
                continue;
            else
                return -1;

        written_bytes += ret;
        n -= written_bytes;
    }

    return written_bytes;
}

int trovaPartner(int pos, char* username, client_info* list){
    int i;
    for (i = 0; i < MAX_USERS; i++){
        if (i != pos && list[i].available == 1 && strcmp(username, list[i].name)== 0){
            return i;
        }
    }
    return -1;
}

void chat_session(int pos, client_info* list) {
    int ret = WriteSocket(list[pos].sock,CHAT_INTRO,strlen(CHAT_INTRO));
    ERROR_HELPER(ret,"ERROR - chat.c line 69");
    ret = WriteSocket(list[pos].sock,CHAT,strlen(CHAT));
    ERROR_HELPER(ret,"ERROR - chat.c line 71");

    char buf[MAX_MESSAGE_LENGTH];
    char messaggio_chat[MAX_MESSAGE_CHAT];
    memset(buf,0,MAX_MESSAGE_LENGTH);
    memset(messaggio_chat,0,MAX_MESSAGE_CHAT);

    while (1){
        ret = ReadSocket(list[pos].sock,buf,MAX_MESSAGE_LENGTH);
        if (errno == EAGAIN){
            ret = WriteSocket(list[pos].sock,"Timeout\n",8);
            ERROR_HELPER(ret,"ERROR - chat.c line 82");
            close_and_cleanup(list[pos].sock,pos,list);
        }
        if(errno == ECONNRESET) {
            close_and_cleanup(list[pos].sock, pos, list);
            pthread_exit(0);
        }
        ERROR_HELPER(ret,"ERROR - chat.c line 89");

        if(list[pos].partner[0] <= 0) return; //break cambiato con return, così la chat session esce proprio

        if (strcmp(buf,"$chat") == 0){
            strcat(buf, " ");
        }
        if (strcmp(buf,"$unchat")==0){
            strcat(buf," ");
        }

        if (strcmp(buf, "$exit") == 0) {
            ret = WriteSocket(list[pos].partner[0],list[pos].name,strlen(list[pos].name));
            ERROR_HELPER(ret, "ERROR - chat.c line 102");
            ret = WriteSocket(list[pos].partner[0],END_CHAT,strlen(END_CHAT));
            ERROR_HELPER(ret, "ERROR - chat.c line 104");
            return; //anche qui break cambiato con return
        }

        strcat(messaggio_chat,"[");
        strcat(messaggio_chat,list[pos].name);
        strcat(messaggio_chat, "]: ");
        strcat(messaggio_chat, buf);
        strcat(messaggio_chat, "\n");
        ret = WriteSocket(list[pos].partner[0], messaggio_chat, MAX_MESSAGE_CHAT);
        if(errno == EBADF) {
            return;
        }
        ERROR_HELPER(ret, "ERROR - chat.c line 117");

        memset(buf,0,MAX_MESSAGE_LENGTH);
        memset(messaggio_chat,0,MAX_MESSAGE_CHAT);

    }
}

