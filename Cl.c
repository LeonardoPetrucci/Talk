#include <WinSock2.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

int main() {

	//integer variable for error management
	int error;

	//connection variables
	LPWSADATA data = { 0 };
	SOCKET connectionSocket;
	SOCKADDR_IN connectionInfo;

	//starting connection to a server, using winsockets
	printf("Connetting, please wait...\n");

	error = WSAStartup(MAKEWORD(2, 2), &data);
	if (error != 0) {
		printf("ERROR: cannot complete WSAStartup operation\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectionSocket == INVALID_SOCKET) {
		printf("ERROR: cannot complete socket operation\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	struct in_addr s;

	/*inet_pton fa la stessa cosa di inet_addr, ovvero traduce una
		stringa che corrisponde in un indirizzo ip, in un valore valido per una struct in_addr*/
	inet_pton(AF_INET, "127.0.0.1", &s); //127.0.0.1 per ora va bene. Poi glielo si deve passare in input. 
	
	connectionInfo.sin_addr = s;
	connectionInfo.sin_family = AF_INET;
	connectionInfo.sin_port = htons(DEFAULT_PORTNUMBER);
	
	error = connect(connectionSocket, (SOCKADDR*)&connectionInfo, sizeof(connectionInfo));
	if (error != 0) {
		printf("ERROR: cannot complete connection operation\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	
	//...

	error = closesocket(connectionSocket);// da sostituire con close_and_clean up.
	if (error != 0) {
		printf("ERROR: cannot complete closesocket operation\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	
	return 0;
}