#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "SharedBuffer.h"

// TODO common: complete implementation of shared_buffer functions

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
        this->__mail[i].key = -1;
        this->__mail[i].is_termination_message = false;
        this->__mail[i].producer_id = -1;
    }
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
int shared_buffer_increase_producer_count(SharedBuffer* this) {
    return 0;
}
/* ---------------------------------------------------------------- */
int shared_buffer_decrease_producer_count(SharedBuffer* this) {
    return 0;
}
/* ---------------------------------------------------------------- */
int shared_buffer_get_consumer_count(SharedBuffer* this) {
    msync(this, this->__bufferSize, MS_SYNC);
    return this->__consumerCount;
}
/* ---------------------------------------------------------------- */
int shared_buffer_increase_consumer_count(SharedBuffer* this) {
    return 0;
}
/* ---------------------------------------------------------------- */
int shared_buffer_decrease_consumer_count(SharedBuffer* this) {
    return 0;
}
/* ---------------------------------------------------------------- */
void shared_buffer_put_message(SharedBuffer* this, int producerId, int key, bool isTerminationMessage) {
    msync(this, this->__bufferSize, MS_SYNC);

    this->__mail[this->__backIndex].producer_id = producerId;
    this->__mail[this->__backIndex].key = key;
    this->__mail[this->__backIndex].is_termination_message = isTerminationMessage;

    this->__backIndex++;
    this->__frontIndex--;
	
    msync(this, this->__bufferSize, MS_SYNC);
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
Message shared_buffer_get_message(SharedBuffer* this, int* messageIndex) {
    Message result;
    return result;
}
/* ---------------------------------------------------------------- */
bool shared_buffer_is_active(SharedBuffer* this) {
    return 0;
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
