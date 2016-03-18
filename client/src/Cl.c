#include <WinSock2.h>
#include <stdio.h>
#include <WS2tcpip.h>

#include "util_windows.h"
#include "..\..\Documents\Visual Studio 2015\Projects\Cl\Cl\util_windows.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {

	//integer variable for error management
	int error;

	if (argc < 2) {
		printf("Non passato indirizzo ip a cui connettersi");
		exit(EXIT_FAILURE);
	}
	
	char ip[256]="";
	sprintf_s(ip, 256, "%s", argv[1]);
	

	//connection variables
	LPWSADATA data = { 0 };
	SOCKET connectionSocket;
	SOCKADDR_IN connectionInfo;

	//starting connection to a server, using winsockets
	printf("Connetting, please wait...\n");

	error = WSAStartup(MAKEWORD(2, 2), &data);
	printf("dopo wsa");
	if (error != 0) {
		printf("ERROR: cannot complete WSAStartup operation\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	printf("prima di socket\n");
	connectionSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectionSocket == INVALID_SOCKET) {
		printf("ERROR: cannot complete socket operation\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	struct in_addr s;
	printf("prima di inet\n");
	/*inet_pton fa la stessa cosa di inet_addr, ovvero traduce una
	stringa che corrisponde in un indirizzo ip, in un valore valido per una struct in_addr*/
	inet_pton(AF_INET, ip, &s); //127.0.0.1 per ora va bene. Poi glielo si deve passare in input. 

	connectionInfo.sin_addr = s;
	connectionInfo.sin_family = AF_INET;
	connectionInfo.sin_port = htons(3000);

	printf("prima di connect");
	error = connect(connectionSocket, (SOCKADDR*)&connectionInfo, sizeof(connectionInfo));
	if (error != 0) {
		printf("ERROR: cannot complete connection operation %d\n", WSAGetLastError());
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	//Qui dovrei creare il nuovo thread che gestisce l' arrivo degli altri messaggi.

	HANDLE thread = LaunchThread(&connectionSocket);

	char buf[MAX_LENGTH];
	int  ret = ReadSocket(connectionSocket, buf, MAX_LENGTH);
	ERROR_HELPER(ret, "Errore nella ricezione del messaggio di errore");
	printf("%s", buf);
	
	while (1) {

		memset(buf, 0, MAX_LENGTH);
		scanf_s("%s", buf);
		ret = WriteSocket(connectionSocket, buf, strlen(buf));
		ERROR_HELPER(ret, "Errore nell' invio dal socket");
		

		ret = ReadSocket(connectionSocket, buf, MAX_LENGTH);
		ERROR_HELPER(ret, "Errore nella ricezione dal socket");

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

		printf("%s", buf);
		memset(buf, 0, MAX_LENGTH);
	}

}
