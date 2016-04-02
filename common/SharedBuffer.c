
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "SharedBuffer.h"

// TODO common: complete implementation of shared_buffer functions

void shared_buffer_init(SharedBuffer* this, int queueSize, size_t bufferSize, char* semaphoreName) {
    strcpy(this->__semaphoreName, semaphoreName);
    this->__backIndex = 0;
    this->__frontIndex = queueSize;
    this->__consumerCount = 0;
    this->__producerCount = 0;
    this->__queueSize = queueSize;
    this->__bufferSize = bufferSize;
    this->__isActive = true;

    int i;
    for (i = 0; i < bufferSize; i++) {
        this->__mail[i].key = 0;
        this->__mail[i].is_termination_message = false;
        this->__mail[i].producer_id = 0;
    }
    msync(this, this->__bufferSize, MS_SYNC);
}

char *shared_buffer_get_semaphore_name(SharedBuffer *this) {
    return NULL;
}

int shared_buffer_get_queue_size(SharedBuffer *this) {
    return 0;
}

int shared_buffer_get_producer_count(SharedBuffer *this) {
    return 0;
}

int shared_buffer_increase_producer_count(SharedBuffer *this) {
    return 0;
}

int shared_buffer_decrease_producer_count(SharedBuffer *this) {
    return 0;
}

int shared_buffer_get_consumer_count(SharedBuffer *this) {
    return 0;
}

int shared_buffer_increase_consumer_count(SharedBuffer *this) {
    return 0;
}

int shared_buffer_decrease_consumer_count(SharedBuffer *this) {
    return 0;
}

void shared_buffer_put_message(SharedBuffer *this, Message message, sem_t semaphore, int* messageIndex) {

}

Message shared_buffer_get_message(SharedBuffer *this, sem_t semaphore, int* messageIndex) {
    Message result;
    return result;
}

bool shared_buffer_is_active(SharedBuffer *this) {
    return 0;
}

void shared_buffer_set_inactive(SharedBuffer *this) {

}
