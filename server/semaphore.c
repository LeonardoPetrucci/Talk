//
// Created by biar on 3/16/16.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "semaphore.h"

int open_semaphore(key_t key, int numero){
    if (numero < 0) return -1;
    return semget(key, numero, IPC_CREAT|0660);
}

int remove_semaphore(int sem_des){
    return semctl(sem_des, 0,IPC_RMID,1);
}

/*int set_all_semaphore(int sem_des,int numero, ushort* array_valori){

    union semun* u1;
    u1->array = array_valori;

    return semctl(sem_des, numero, SETALL, u1);
} u1->array MI ROSICA*/

int set_semaphore(int sem_des, int num_sem, int valore){
    return semctl(sem_des,num_sem, SETVAL, valore);
}

int sem_wait(int sem_des, int num_sem){
    struct sembuf operazioni[1] = {{num_sem,-1,0}};
    return semop(sem_des,operazioni,1);
}

int sem_signal(int sem_des, int num_sem){
    struct sembuf operazioni[1] = {{num_sem,+1,0}};
    return semop(sem_des,operazioni,1);
}