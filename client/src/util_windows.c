#include <stdio.h>
#include <Windows.h>
#include <winsock.h>
#include "util_windows.h"


size_t ReadSocket(int ds, char buf[], int n) {
	size_t read_bytes = 0;
	int ret;
	while (read_bytes <= n) {
		ret = recv(ds, buf + read_bytes , n, 0);
		ERROR_HELPER(ret, "Error in recv in ReadSocket");
		if (ret == 0) break;

		read_bytes += ret;
		if (buf[read_bytes] <= 0) break;
		n -= ret;
	}
	buf[read_bytes] = '\0';
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
		//buf[written_bytes] = '\n';
		n -= written_bytes;
	}

	return written_bytes;
}

void close_and_cleanup(SOCKET socket) {
	closesocket(socket);
	WSACleanup();
	ExitProcess(EXIT_SUCCESS);

}