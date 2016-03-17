#include <stdio.h>
#include <Windows.h>
#include <winsock.h>
#include "util_windows.h"


size_t ReadSocket(int ds, char buf[], int n) {
	size_t read_bytes = 0;
	int ret;
	while (1) {
		ret = recv(ds, buf + read_bytes , n, 0);
		ERROR_HELPER(ret, "Error in recv in ReadSocket");
		if (ret == 0) break;

		read_bytes += ret;
		n -= ret;
	}

	return read_bytes;
}

size_t	WriteSocket(int ds, char buf[], int n) {
	size_t written_bytes = 0;
	int ret;
	while (1) {
		ret = send(ds, buf + written_bytes, n, 0);
		ERROR_HELPER(ret, "Error in send in WriteSocket");
		if (ret == 0) break;

		written_bytes += ret;
		n -= written_bytes;
	}

	return written_bytes;
}

void close_and_cleanup(SOCKET socket) {
	
	closesocket(socket);
	//free(args);
	WSACleanup();
	ExitProcess(EXIT_SUCCESS);

}