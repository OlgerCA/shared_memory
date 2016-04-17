#ifndef SHARED_MEMORY_SEMAPHORE_H
#define SHARED_MEMORY_SEMAPHORE_H

char* shared_semaphore = "/shared_sem18";

sem_t* create_shared_semaphore(char* sem_name);
sem_t* get_shared_semaphore(char* sem_name);

#endif //SHARED_MEMORY_SEMAPHORE_H
