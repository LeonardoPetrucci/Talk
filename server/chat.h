//
// Created by Leonardo on 15/03/2016.
//

#ifndef TALK_CHAT_H
#define TALK_CHAT_H

#include "structs.h"
/*
	Read and return the number of bytes read from the socket
*/
size_t  ReadSocket(int ds, char buf[], int n);
/*
	Write and return the number of bytes written from the socket
*/
size_t  WriteSocket(int ds, char buf[], int n);

// Funzione ausiliaria che trova dato lo username il partner per una chat_session

int trovaPartner(char username[], client_info* list);

// Esegue tutte le operazioni preliminari per l'inizio di una chat session

//void startChatSession(int sock,client_info* list, int pos, char username[]);



void chat_session();

int initialize_queue(int key, int flag);

int enqueue(char* nickname, char* msg, int queue);

msg_t* dequeue();

void dequeue_routine();

#endif //TALK_CHAT_H
