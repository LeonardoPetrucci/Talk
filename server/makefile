CC=gcc
LFLAGS= -std=c99 -lpthread


server: server.o commands.o msg_queue.o semaphore.o chat.o
	$(CC) server.o commands.o msg_queue.o semaphore.o chat.o $(LFLAGS)

server.o: server.c structs.h macros.h messages.h chat.h
	$(CC) -c server.c 

chat.o: chat.c 
	$(CC) -c chat.c 

commands.o: commands.c 
	$(CC) -c commands.c

msg_queue.o: msg_queue.c 
	$(CC) -c msg_queue.c

semaphore.o: semaphore.c
	$(CC) -c semaphore.c

