CC = gcc
CFLAGS =-Wall -O0 -g
LDFLAGS= -std=c99 -lpthread


server: messages.h chat.h commands.h threads.h macros.h semaphore.h structs.h server.c chat.c threads.c semaphore.c
	rm -f build/*.o
	$(CC) -c server.c -o build/server.o
	$(CC) -c commands.c -o build/commands.o
	$(CC) -c chat.c -o build/chat.o
	$(CC) -c threads.c -o build/threads.o
	$(CC) -c semaphore.c -o build/semaphore.o
	$(CC) -o server build/*.o $(LDFLAGS)
