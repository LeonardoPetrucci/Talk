#include <sys/types.h>

#ifndef UTIL_CLIENT_SEMAPHORE_H
#define UTIL_CLIENT_SEMAPHORE_H

extern int sem_list;

/*
 * Crea un semaforo.
 */
int open_semaphore(key_t key, int numero);
/*
 * Rimuove un semafore dal sistema
 */
int remove_semaphore(int sem_des);
/*
 * setta i valori di tutti i semafori dell'array. numero è il numero dei semafori.
 */
int set_all_semaphore(int sem_des,int numero, ushort* array_valori);
/*
 * Setta il valore del semaforo specificato
 */
int set_semaphore(int sem_des, int num_sem, int valore);
/*
 * Wait sul semaforo
 */
int sem_wait(int sem_des, int num_sem);
/*
 * Signal sul semaforo
 */
int sem_signal(int sem_des, int num_sem);

#endif //UTIL_CLIENT_SEMAPHORE_H
