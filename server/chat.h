//
// Created by Leonardo on 15/03/2016.
//

#ifndef TALK_CHAT_H
#define TALK_CHAT_H


/*
	Read and return the number of bytes read from the socket
*/
size_t  ReadSocket(int ds, char buf[], int n);
/*
	Write and return the number of bytes written from the socket
*/
size_t  WriteSocket(int ds, char buf[], int n);
/*
 * Gestisce la chat_session tra due utenti. Avrà minimo due parametri in ingresso che sono le struct con le informazioni sugli user.
 */
void chat_session(chat_args* args);

/*
 * Inizializza la coda di messaggi per la chat_session corrente.Key unico nel sistema.
 */
int initialize_queue(int key);

/*
 * Inserisce nella coda di messaggi un messaggio.
 */
int enqueue(char* nickname, char* msg);

/*
 * Leva dalla coda un messaggio
 */
msg_t* dequeue();

/*
 * Funzione thread. Routine che lancia dequeue
 */
void dequeue_routine();

/*
 * Invia il messaggio tolto dalla coda al destinatario.
 */

#endif //TALK_CHAT_H
