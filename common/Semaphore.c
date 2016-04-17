#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include "Semaphore.h"

sem_t* get_shared_semaphore(char* sem_name){
    sem_t* semaphore = sem_open(sem_name, 0, 0644, 0);
    if (semaphore == SEM_FAILED) {
        perror("Error creating semaphore");
        return (sem_t *) NULL;
    }
    return semaphore;
}

sem_t* create_shared_semaphore(char* sem_name){
    sem_t* semaphore = sem_open(sem_name, O_CREAT, 0644, 1);
    if (semaphore == SEM_FAILED) {
        perror("Error creating semaphore");
        return (sem_t *) NULL;
    }
    return semaphore;
}