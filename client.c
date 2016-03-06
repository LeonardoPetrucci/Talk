#include <WinSock2.h>
#include <stdio.h>
 

#pragma comment(lib, "Ws2_32.lib")

int main() {

	//integer variable for error management
	int error;

	//required user information
	clientInfo user;

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

	connectionInfo.sin_family = AF_INET;
	connectionInfo.sin_port = htons(DEFAULT_PORTNUMBER);
	connectionInfo.sin_addr.S_un.S_un_b.s_b1 = 127;
	connectionInfo.sin_addr.S_un.S_un_b.s_b2 = 0;
	connectionInfo.sin_addr.S_un.S_un_b.s_b3 = 0;
	connectionInfo.sin_addr.S_un.S_un_b.s_b4 = 1;

	error = connect(connectionSocket, (SOCKADDR*)&connectionInfo, sizeof(connectionInfo));
	if (error != 0) {
		printf("ERROR: cannot complete connection operation\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	//now we are in the main room. We need to choose a nickname, obtain the user list, choose a available partner.
	printf("CONNECTED! Nice to meet you. first of all, you need to create a nickname (max. 20 characters)\nNICKNAME: ");
	char* buffer = (char*)malloc(sizeof(char));
	fgets(buffer, MAX_NAME_SIZE, stdin);
	user.userNickname = setNickname(buffer, connectionSocket);
	user.avaiable = TRUE;
	user.partnerNickname = NULL;

	printf("Now you are in the main room: here you can:\n-Type :lista to obtain the user list to select a partner for a chat\n-Type :connect to connect to another selected user\nIf you need a full command list you can type :help. If you want to exit from the main room you can type :quit.\n");

	char usersList[MAXLIST];
	strcpy_s(usersList, MAXLIST, "---nobody---\n");
	int len = strlen(usersList);

	error = recv(connectionSocket, usersList, len, 0);
	if (error == SOCKET_ERROR) {
		printf("errore nella receive della lista con codice %d\n", WSAGetLastError());
	}

	printf("%s", usersList);

	char *cmd;
	cmd = ":lista";

	while (1) {
		error = send(connectionSocket, cmd, strlen(cmd), 0);
		if (error == SOCKET_ERROR) printf("Errore nella send di cmd con codice %d", WSAGetLastError());

		if (sizeof(cmd) > MAX_MESSAGE_LENGTH) {
			printf("[RETRY] the message is too long. It must be less or equal tha 128 characters");
		}
		//con la send invio il comando al server da qui non so come continuare. in caso di connessione faccio partire un thread?
	}

	error = closesocket(connectionSocket);
	if (error != 0) {
		printf("ERROR: cannot complete closesocket operation\n");
		WSACleanup();
		exit(EXIT_FAILURE);
	}



	return 0;
}