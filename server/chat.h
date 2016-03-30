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

int trovaPartner(int pos, char username[], client_info* list);

void chat_session(int pos, client_info* list);

#endif //TALK_CHAT_H
