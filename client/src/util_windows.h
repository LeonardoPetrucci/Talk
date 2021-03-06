#include <Windows.h>

#define DEFAULT_PORTNUMBER 3000
#define MAX_LENGTH 1024

#define ERROR_HELPER(ret, message)  do {											\
            if (ret == SOCKET_ERROR) {                                              \
                fprintf(stderr, "%s: %s\n", message, WSAGetLastError());			\
                ExitProcess(EXIT_SUCCESS);											\
            }																		\
        } while (0)																	\

#define QUITMESS        "You are going to be disconnected. We hope to see you again!\n"
#define KILL_CLIENT     "The server is no more available. Please try later!\n"
//var that indicates if you are in chat or not
extern int chat; 
/*
	util defines a sets of functions that can be used by server and client
*/

/*
	Read and return the number of bytes read from the socket
*/
size_t ReadSocket(int ds, char buf[], int n); 

/*
	Write and return the number of bytes written from the socket
*/
size_t	WriteSocket(int ds, char buf[], int n);

/*
	Chiude e libera le risorse del socket.
*/
void close_and_cleanup(SOCKET socket);
/*
*	Crea il thread che � sempre in ascolto sul socket
*/
HANDLE LaunchThread(int* ds);
/*
* Thread sempre in ascolto sul socket.
*/
void ListenSocket(void* arg);