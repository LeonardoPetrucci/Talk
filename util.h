/*
	util defines a sets of functions that can be used by server and client
*/

#include <stdio.h>

#define MAXMESSAGESIZE 32
#define LIST "$list"
#define QUIT "$quit"
#define HELP "$help"
#define QUIT_CONNECTION "$quit_connection"

#define CONNECTION "$connection"
#define SIZE_CONNECTION 11

#define HELPMESSAGE "Puoi mandare questi comandi:\n$list - invia la lista degli utenti disponibili per una conversazione\n$quit - esce dalla chat\n$connection USERNAME - inizia una chat session con l'utente USERNAME se possibile\n$quit_connection - permette di uscire da una chat session\n"

#define ERROR_HELPER(ret, message) do{                              \
    if (ret < 0){                                                   \
        fprintf(stderr, "%s: %s\n", message, strerror(errno));      \
        exit(EXIT_FAILURE);                                         \
    }                                                               \
} while(0)

#define MAXLUNNICK 20
#define MAXMSG 1024

typedef struct msg_s {
    char mittente[MAXLUNNICK];
    char msg[MAXMSG];
} msg_t;
/*
	Read and return the number of bytes read from the socket
*/
size_t ReadSocket(int ds, char buf[], int n);

/*
	Write and return the number of bytes written from the socket
*/
size_t	WriteSocket(int ds, char buf[], int n);

/*
 * Manage closure and cleanup of resources
 */
void close_and_cleanup(int socket);

/*
 * Manage special command between client and server
 */
void cmdManagement(int socket);

/*
 * It sends utents-connected-and-avalaible list. It returns the number of bytes sent.
 */

size_t sendList(int socket);

/*
 * Gestisce la chat_session tra due utenti. Avrà minimo due parametri in ingresso che sono le struct con le informazioni sugli user.
 */
void chat_session();

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
