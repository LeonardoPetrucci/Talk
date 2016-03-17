#define DEFAULT_PORTNUMBER 3000
#define MAX_LENGTH 1024

#define ERROR_HELPER(ret, message)  do {											\
            if (ret == SOCKET_ERROR) {                                              \
                fprintf(stderr, "%s: %s\n", message, WSAGetLastError());			\
                ExitProcess(EXIT_SUCCESS);											\
            }																		\
        } while (0)																	\

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