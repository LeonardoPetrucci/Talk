#include <WinSock2.h>
#include <stdio.h>
#include <WS2tcpip.h>

#include "util_windows.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {

	//integer variable for error management
	int error;

/*	if (argc < 1) {
		printf("Non passato indirizzo ip a cui connettersi");
		exit(EXIT_FAILURE);
	}

	char*  ip = "";
	strcpy_s(ip, strlen(argv[0]), argv[0]);
	*/

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
	inet_pton(AF_INET, "192.168.56.1", &s); //127.0.0.1 per ora va bene. Poi glielo si deve passare in input. 

	connectionInfo.sin_addr = s;
	connectionInfo.sin_family = AF_INET;
	connectionInfo.sin_port = htons(3000);

	error = connect(connectionSocket, (SOCKADDR*)&connectionInfo, sizeof(connectionInfo));
	if (error != 0) {
		printf("ERROR: cannot complete connection operation %d\n", WSAGetLastError());
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	char buf[MAX_LENGTH];
	int  ret = ReadSocket(connectionSocket, buf, MAX_LENGTH);
	ERROR_HELPER(ret, "Errore nella ricezione del messaggio di errore");
	printf("%s", buf);
	
	while (1) {

		scanf_s("%s", buf);
		ret = WriteSocket(connectionSocket, buf, strlen(buf));
		ERROR_HELPER(ret, "Errore nell' invio dal socket");

		ret = ReadSocket(connectionSocket, buf, MAX_LENGTH);
		ERROR_HELPER(ret, "Errore nella ricezione dal socket");

	}

	close_and_cleanup(connectionSocket);

	return 0;
}