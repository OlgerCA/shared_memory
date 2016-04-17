#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SharedBuffer.h"

// TODO common: complete implementation of shared_buffer functions


void printMessageAdded(SharedBuffer *this, int index);


int shared_buffer_open_file(char* bufferName) {
    return open(bufferName, O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0600);
}
/* ---------------------------------------------------------------- */
size_t shared_buffer_get_size(int fileDesc, size_t queueSize) {
    __off_t offset = lseek(fileDesc, sizeof(SharedBuffer) + sizeof(Message) * queueSize - 1, SEEK_SET);
    if (offset == -1) {
        return (size_t) offset;
    }
    
    return (size_t) (offset + 1);
}

/* ---------------------------------------------------------------- */
SharedBuffer* shared_buffer_load(int fileDesc, size_t mapSize) {
    SharedBuffer* buffer = (SharedBuffer*) mmap(
        NULL,
        mapSize,
        PROT_EXEC | PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fileDesc,
        0
    );
    
    return buffer;
}
/* ---------------------------------------------------------------- */


SharedBuffer * shared_buffer_get(char *buffer_name){
    int fd = open(buffer_name, O_RDWR, (mode_t)0600);
    if (fd == -1) {
        perror("Error opening file for writing");
        exit(-1);
    }

    off_t offset = lseek(fd, (size_t)0, SEEK_END);
    if (offset == -1) {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        exit(-1);
    }

    size_t map_size = (size_t) (offset + 1);

    SharedBuffer* buffer = (SharedBuffer*) mmap(NULL, map_size
            ,PROT_EXEC | PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
        close(fd);
        perror("Error mapping file");
        exit(-1);
    }
    return buffer;
}


void shared_buffer_init(SharedBuffer* this, int queueSize, size_t bufferSize, char* semaphoreName) {
    strcpy(this->__semaphoreName, semaphoreName);
    this->__backIndex = 0;
    this->__frontIndex = queueSize;
    this->__consumerCount = 0;
    this->__producerCount = 0;
    this->__queueSize = queueSize;
    this->__bufferSize = bufferSize;
    this->__isActive = true;
    this->__bufferSize = 0;

    msync(this, this->__bufferSize, MS_SYNC);
}

/* ---------------------------------------------------------------- */
char* shared_buffer_get_semaphore_name(SharedBuffer* this) {
    return this->__semaphoreName;
}
/* ---------------------------------------------------------------- */
sem_t* shared_buffer_open_semaphore(char* semaphoreName) {
    return sem_open(semaphoreName, O_CREAT /*| O_EXCL*/, S_IRWXO, 0);
}
/* ---------------------------------------------------------------- */
int shared_buffer_get_queue_size(SharedBuffer* this) {
    return this->__queueSize;
}

/* ---------------------------------------------------------------- */
int shared_buffer_get_producer_count(SharedBuffer* this) {
    msync(this, this->__bufferSize, MS_SYNC);
    return this->__producerCount;
}
/* ---------------------------------------------------------------- */

int shared_buffer_increase_producer_count(SharedBuffer *this, sem_t *semaphore) {
    int currentCount;
    sem_wait(semaphore);
    this->__producerCount++;
    currentCount = this->__producerCount;
    sem_post(semaphore);
    return currentCount;
}

int shared_buffer_decrease_producer_count(SharedBuffer *this, sem_t *semaphore) {
    int currentCount;
    sem_wait(semaphore);
    this->__producerCount--;
    currentCount = this->__producerCount;
    sem_post(semaphore);
    return currentCount;
}

/* ---------------------------------------------------------------- */
int shared_buffer_get_consumer_count(SharedBuffer* this) {
    msync(this, this->__bufferSize, MS_SYNC);
    return this->__consumerCount;
}

int shared_buffer_increase_consumer_count(SharedBuffer *this, sem_t *semaphore) {
    int currentCount;
    sem_wait(semaphore);
    this->__consumerCount++;
    currentCount = this->__producerCount;
    sem_post(semaphore);
    return currentCount;
}

int shared_buffer_decrease_consumer_count(SharedBuffer *this, sem_t *semaphore) {
    int currentCount;
    sem_wait(semaphore);
    this->__consumerCount--;
    currentCount = this->__producerCount;
    sem_post(semaphore);
    return currentCount;
}
/* ---------------------------------------------------------------- */

double shared_buffer_put_message(SharedBuffer* this, Message message, sem_t *semaphore) {
    clock_t start = clock(), diff;
    sem_wait(semaphore);
    diff = clock() - start;
    msync(this, this->__bufferSize, MS_SYNC);
    int index;

    index = this->__messageCount % this->__queueSize;
    this->__mail[index] = message;
    this->__messageCount++;

    this->__backIndex++;
    this->__frontIndex--;
    msync(this, this->__bufferSize, MS_SYNC);

    sem_post(semaphore);
    printMessageAdded(this, index);

    return diff;
}

/* ---------------------------------------------------------------- */
void shared_buffer_put_stop(SharedBuffer* this, int consumerId) {
    msync(this, this->__bufferSize, MS_SYNC);

    this->__mail[this->__backIndex].key = consumerId % 5;
    this->__mail[this->__backIndex].is_termination_message = true;

    this->__backIndex++;
    this->__frontIndex--;

    msync(this, this->__bufferSize, MS_SYNC);
}
/* ---------------------------------------------------------------- */
Message shared_buffer_get_message(SharedBuffer *this, sem_t *semaphore, int* messageIndex) {
    Message result;
    sem_wait(semaphore);
    int indexRead = this->__readCount % this->__queueSize;

    if(this->__readCount < this->__messageCount){
        result = this->__mail[indexRead];
    }

    this->__readCount++;
    sem_post(semaphore);
    return result;
}

/* ---------------------------------------------------------------- */
void shared_buffer_set_inactive(SharedBuffer* this) {
    this->__isActive = false;
    msync(this, this->__bufferSize, MS_SYNC);
}
/* ---------------------------------------------------------------- */
void shared_buffer_close_file(int fileDesc) {
    close(fileDesc);
}
/* ---------------------------------------------------------------- */
void shared_buffer_unlink(SharedBuffer* this, int fileDesc, size_t mapSize) {
    if (this != MAP_FAILED) {
        munmap(this, mapSize);
    }
    close(fileDesc);
}

bool shared_buffer_is_active(SharedBuffer *this) {
    return this->__isActive;
}

void printMessageAdded(SharedBuffer *this, int index) {
    printf("Action: New Message added:\n");
    printf("\tMessage index: %d\n", index);
    printf("\tLive Producers: %d\n", this->__producerCount);
    printf("\tLive Consumers: %d\n", this->__consumerCount);

}
