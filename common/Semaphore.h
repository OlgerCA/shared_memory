#ifndef SHARED_MEMORY_SEMAPHORE_H
#define SHARED_MEMORY_SEMAPHORE_H

char *buffer_sem_name = "/buffer_sem";
char *fill_sem_name = "/fill_sem";
char *empty_sem_name = "/empty_sem";

sem_t* create_shared_semaphore(char* sem_name, int initial_value);
sem_t* get_shared_semaphore(char* sem_name);

#endif //SHARED_MEMORY_SEMAPHORE_H
