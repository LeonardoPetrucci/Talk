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