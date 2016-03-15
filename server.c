//
// Created by Leonardo on 15/03/2016.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#include "structs.h"
#include "macros.h"
#include "messages.h"
#include "chat.h"
#include "commands.h"
/*  SERVER
 *  Il server deve tenere conto delle seguenti informazioni:
 *      - Lista dei client connessi disponibili (da mostrare, implementata tramite array)
 *      - Socket di ascolto
 *      - Indirizzo IP
 *      - Porta TCP
 *
 *  IMPORTANTISSIMO: CODICE SCRITTO IN C99! per compilare aggiungere alla riga -std=C99
 */
int connections = 0;

/*  Thread di gestione della connessione
 *  Il thread deve gestire l'inserimento del client appena connesso nella lista di client connessi, utilizzando
 *  opportuni semafori per evitare problemi di sincronizzazione di questa. Dopo che sarà verificata la disponibilità
 *  di connessione tramite confronto con il numero connections, verrà chiesto di scegliere un nome utente.
 *  Settato il nome utente, verrà inviata al client la lista dei client connessi disponibili.
 */
void* _connection_handler(int socket_descriptor) {
    puts("THREAD creato correttamente\n"); //messaggio al solo scopo di debugging

    int     n, position = -1;
    char    buffer[MAX_MESSAGE_LENGTH];
    char    name[MAX_NAME_LENGTH];

    bzero(buffer, sizeof(buffer));
    bzero(name, sizeof(name));
    //assuzione che tutto vada bene, assoluamente da cambiare!!!
    n = WriteSocket(socket_descriptor, WELCOME, sizeof(WELCOME));
    n = ReadSocket(socket_descriptor, name, sizeof(name));
    for(int i = 0; i < MAX_USERS; i++) {
        if(clist[i].sock < 0) {
            //SEZIONE CRITICA, PROTEGGI CON SEMAFORO!!!
            clist[i].sock = socket_descriptor;
            memcpy(clist[i].username, name, MAX_NAME_LENGTH);
            clist[i].partner = -1;
            clist[i].available = 1;
            break;
        }
        position = i;
        break;
    }
    if(position < 0) {
        printf("non ho assegnato nessuna posizione");
        //chiudere la connessione con il server perchè non c'è posto nella lista
    }
    sendList(socket_descriptor, clist, position);
    exit(EXIT_SUCCESS);
}

int main() {
    /*Dichiarazione variabili utili*/
    int     listening_socket;
    struct  sockaddr_in server_address;
    int     connection_socket;
    struct  sockaddr_in client_address;
    int     port = 3000; //cambiare gestione porta
    int     sin_length;

    /*Pulizia delle strutture utilizzate*/
    memset((void*)&clist, 0, sizeof(clist));
    memset((void*)&server_address, 0, sizeof(server_address));
    memset((void*)&client_address, 0, sizeof(client_address));

    /*Costruzione impalcatura di connessione*/
    for(int i = 0; i < MAX_USERS; i++) {
        clist[i].sock = -2;
    }

    if((listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("ERRORE: operazione socket non riuscita"); //sostituisci i perror con system call gestite tramite errno
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); //cambiare
    server_address.sin_port = htons(3000); //cambiare

    if(bind(listening_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("ERRORE: operazione bind non riuscita");
        exit(EXIT_FAILURE);
    }

    printf("Server running...\n");
    listen(listening_socket, CONNECTION_QUEUE);
    while(1) {
        sin_length = sizeof(struct sockaddr_in);
        if((connection_socket = accept(listening_socket, (struct sockaddr*)&client_address, &sin_length)) < 0) {
            perror("ERRORE: operazione accept non riuscita");
            exit(EXIT_FAILURE);
        }
        puts("CONNESSO CON UN CLIENT\n");
        pthread_t* ch;
        pthread_create(&ch, NULL, _connection_handler, connection_socket);

    }
}