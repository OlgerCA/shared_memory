#ifndef SHARED_MEMORY_SHAREDBUFFER_H
#define SHARED_MEMORY_SHAREDBUFFER_H

#include <semaphore.h>
#include "Message.h"

typedef struct {
    char __semaphoreName[16];
    int __queueSize;
    size_t __bufferSize;
    int __producerCount;
    int __consumerCount;
    int __frontIndex;
    int __backIndex;
    bool __isActive;
    int __messageCount;
    int __readCount;
    Message __mail[];
} SharedBuffer;

int shared_buffer_open_file(char* bufferName);
size_t shared_buffer_get_size(int fileDesc, size_t queueSize);
SharedBuffer* shared_buffer_load(int fileDesc, size_t mapSize);
SharedBuffer* shared_buffer_get(char *buffer_name);
void shared_buffer_init(SharedBuffer* this, int queueSize, size_t bufferSize, char* semaphoreName);
char* shared_buffer_get_semaphore_name(SharedBuffer* this);
sem_t* shared_buffer_open_semaphore(char* semaphoreName);
int shared_buffer_get_queue_size(SharedBuffer* this);
int shared_buffer_get_producer_count(SharedBuffer* this);
int shared_buffer_increase_producer_count(SharedBuffer* this, sem_t *semaphore);
int shared_buffer_decrease_producer_count(SharedBuffer* this, sem_t *semaphore);
int shared_buffer_get_consumer_count(SharedBuffer* this);

double shared_buffer_put_message(SharedBuffer* this, Message message, sem_t *semaphore);
void shared_buffer_put_stop(SharedBuffer* this, int consumerId);
Message shared_buffer_get_message(SharedBuffer* this, sem_t *semaphore, int* messageIndex);

int shared_buffer_increase_consumer_count(SharedBuffer* this, sem_t *semaphore);
int shared_buffer_decrease_consumer_count(SharedBuffer* this, sem_t *semaphore);


bool shared_buffer_is_active(SharedBuffer* this);
void shared_buffer_set_inactive(SharedBuffer* this);
void shared_buffer_close_file(int fileDesc);
void shared_buffer_unlink(SharedBuffer* this, int fileDesc, size_t mapSize);

#endif
