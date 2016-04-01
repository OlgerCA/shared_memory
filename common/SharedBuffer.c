
#include <stdio.h>
#include "SharedBuffer.h"

// TODO common: complete implementation of shared_buffer functions

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

void shared_buffer_set_active(SharedBuffer *this) {

}

void shared_buffer_set_inactive(SharedBuffer *this) {

}
