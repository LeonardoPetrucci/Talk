//
// Created by Leonardo on 15/03/2016.
//
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

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

int trovaPartner(char username[], client_info* list){
    int i;
    for (i = 0; i < MAX_USERS; i++){
        if (strcmp(list[i].name, username)== 0){
            return i;
        }
    }
    return -1;
}

void startChatSession(int socket,client_info* list, int pos, char username[]) {

    int ret;

    //devo trovare la struct info con quello username
    int  altro_pos = trovaPartner(username,list);

    if (altro_pos == -1){
        char* not_found = "Utente non trovato\n";
        ret = WriteSocket(socket,not_found,strlen(not_found));
        ERROR_HELPER(ret, "Errore in 'Utente non Trovato'");
        return;
    }

    char* found="";
    sprintf(found,"Utente %s trovato! Ti mando in chat_session!\n",list[pos].name);
    ret = WriteSocket(socket, found,strlen(found));

    //lancio funzione che gestisce la chat session..

    //devo inviare segnale a thread partner
    //PROSEGUI DA QUI

}
