#include <WinSock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include <wincon.h>
#include "util_windows.h"

#pragma comment(lib, "Ws2_32.lib")

SOCKET connectionSocket;
int chat = 0;

void interruptManagement(DWORD fdwCtrlType) {
	if (fdwCtrlType == CTRL_C_EVENT || fdwCtrlType == CTRL_BREAK_EVENT || fdwCtrlType == CTRL_CLOSE_EVENT || fdwCtrlType == CTRL_LOGOFF_EVENT || fdwCtrlType == CTRL_SHUTDOWN_EVENT) {
		if(chat){
			int ret = WriteSocket(connectionSocket, "$exit", 5);
			ERROR_HELPER(ret, "Error in sending exit");
		}

		if (connectionSocket > 0) {
			printf("\nA CTRL+C event was sent. I'm logging you off.\n");
			int ret = WriteSocket(connectionSocket, "$quit", 5);
			ERROR_HELPER(ret, "Error in sending $quit");

			char buf[MAX_LENGTH];
			ret = ReadSocket(connectionSocket, buf, MAX_LENGTH);
			ERROR_HELPER(ret, "Error in recv quit message");
		}

		close_and_cleanup(connectionSocket);
	}
}


int main(int argc, char* argv[]) {

	//integer variable for error management
	int error;

	if (argc < 2) {
		printf("Non passato indirizzo ip a cui connettersi");
		exit(EXIT_FAILURE);
	}
	
	//connection variables
	LPWSADATA data = { 0 };
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
	inet_pton(AF_INET, argv[1], &s); //127.0.0.1 per ora va bene. Poi glielo si deve passare in input. 

	connectionInfo.sin_addr = s;
	connectionInfo.sin_family = AF_INET;
	connectionInfo.sin_port = htons(3000);

	error = connect(connectionSocket, (SOCKADDR*)&connectionInfo, sizeof(connectionInfo));
	if (error != 0) {
		printf("ERROR: cannot complete connection operation %d\n", WSAGetLastError());
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)interruptManagement, TRUE);

	//Qui dovrei creare il nuovo thread che gestisce l' arrivo degli altri messaggi.

	HANDLE thread = LaunchThread(&connectionSocket);

	char buf[MAX_LENGTH];
	int ret = ReadSocket(connectionSocket, buf, MAX_LENGTH);
	ERROR_HELPER(ret, "Errore nella ricezione del messaggio di errore");
	printf("%s", buf);
	
	while (1) {

		memset(buf, 0, MAX_LENGTH);
		gets_s(buf, sizeof(buf));

		ret = WriteSocket(connectionSocket, buf, strlen(buf));
		ERROR_HELPER(ret, "Errore nell' invio dal socket");
		
	}

	close_and_cleanup(connectionSocket);

	return 0;
}

HANDLE LaunchThread(int* ds) {
	
	HANDLE hthread; DWORD hid;

	hthread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE*)ListenSocket, (void*)ds, NORMAL_PRIORITY_CLASS, &hid);

	if (hthread == NULL)	printf("Chiamata fallita\n");

	return hthread;
}


void ListenSocket(void * arg) {
	char buf[MAX_LENGTH]; int *ds = (int*)arg;
	while (1) {
		
		int ret = ReadSocket(*ds, buf, MAX_LENGTH);
		ERROR_HELPER(ret, "Errore nella read di ListenSocket");

		
		if (ret < 0) {
			printf("errore nel kill timer\n");
			close_and_cleanup(*ds);
		}
		

		if (strcmp("$chat", buf) == 0) {
			chat = 1;
			continue;
		}
		if (strcmp("$unchat", buf) == 0) {
			chat = 0;
			continue;
		}

		printf("%s", buf);
		memset(buf, 0, MAX_LENGTH);
	}

}
