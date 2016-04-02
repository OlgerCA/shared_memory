
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
    Message __mail[];
} SharedBuffer;

void shared_buffer_init(SharedBuffer* this, int queueSize, size_t bufferSize, char* semaphoreName);
char* shared_buffer_get_semaphore_name(SharedBuffer* this);
int shared_buffer_get_queue_size(SharedBuffer* this);
int shared_buffer_get_producer_count(SharedBuffer* this);
int shared_buffer_increase_producer_count(SharedBuffer* this);
int shared_buffer_decrease_producer_count(SharedBuffer* this);
int shared_buffer_get_consumer_count(SharedBuffer* this);
int shared_buffer_increase_consumer_count(SharedBuffer* this);
int shared_buffer_decrease_consumer_count(SharedBuffer* this);
void shared_buffer_put_message(SharedBuffer* this, Message message, sem_t semaphore, int* messageIndex);
Message shared_buffer_get_message(SharedBuffer* this, sem_t semaphore, int* messageIndex);
bool shared_buffer_is_active(SharedBuffer* this);
void shared_buffer_set_inactive(SharedBuffer* this);

#endif
