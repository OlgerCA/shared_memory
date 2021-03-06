#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SharedBuffer.h"
#include "Message.h"

int shared_buffer_open_file(char* bufferName) {
	return open(bufferName, O_RDWR, (mode_t) 0600);
}
/* ---------------------------------------------------------------- */
size_t shared_buffer_get_size(int fileDesc, size_t queueSize) {
	__off_t offset = lseek(fileDesc, (size_t) 0, SEEK_END);
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
SharedBuffer* shared_buffer_get(char *buffer_name){
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
/* ---------------------------------------------------------------- */
void shared_buffer_init(SharedBuffer* this, int queueSize, size_t bufferSize, char *buffer_sem_name, char *fill_sem_name, char *empty_sem_name) {
	strcpy(this->__buffer_sem_name, buffer_sem_name);
	strcpy(this->__fill_sem_name, fill_sem_name);
	strcpy(this->__empty_sem_name, empty_sem_name);
	this->__backIndex = 0;
	this->__frontIndex = queueSize;
	this->__consumerCount = 0;
	this->__producerCount = 0;
	this->__queueSize = queueSize;
	this->__bufferSize = bufferSize;
	this->__isActive = true;
	this->__bufferSize = 0;
	
	int i = 0;
	for(; i < queueSize; i++){
		this->__mail[i].readFlag = true;
	}
	
	msync(this, this->__bufferSize, MS_SYNC);
}
/* ---------------------------------------------------------------- */
char* shared_buffer_get_semaphore_name(SharedBuffer* this) {
	return this->__buffer_sem_name;
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
/* ---------------------------------------------------------------- */
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
/* ---------------------------------------------------------------- */
int shared_buffer_increase_consumer_count(SharedBuffer *this, sem_t *semaphore) {
	int currentCount;
	sem_wait(semaphore);
	this->__consumerCount++;
	currentCount = this->__producerCount;
	sem_post(semaphore);
	return currentCount;
}
/* ---------------------------------------------------------------- */
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
	bool successfulMessage = false;
	
	index = this->__messageCount % this->__queueSize;
	
	if(this->__mail[index].readFlag == true){
		this->__mail[index] = message;
		this->__messageCount++;
		successfulMessage = true;
	}
	
	msync(this, this->__bufferSize, MS_SYNC);
	
	sem_post(semaphore);
	if(successfulMessage)
		printMessageAdded(this, index, message);
	
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
Message shared_buffer_get_message(SharedBuffer *this, sem_t *semaphore) {
	Message result;
	clock_t start = clock(), diff;
	sem_wait(semaphore);
	msync(this, this->__bufferSize, MS_SYNC);
	diff = clock() - start;
	result.semWaitTime = diff;
	result.key = -1;
	int indexRead = this->__readCount % this->__queueSize;
	
	if(this->__readCount < this->__messageCount){
		result = this->__mail[indexRead];
		result.semWaitTime = diff;
		this->__mail[indexRead].readFlag = true;
		this->__readCount++;
	}
	msync(this, this->__bufferSize, MS_SYNC);
	sem_post(semaphore);
	if(result.key != -1) printMessageRead(this, indexRead, result);
	return result;
}
/* ---------------------------------------------------------------- */
void shared_buffer_set_inactive(SharedBuffer* this) {
	this->__isActive = 0;
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
/* ---------------------------------------------------------------- */
bool shared_buffer_is_active(SharedBuffer *this) {
	return this->__isActive;
}
/* ---------------------------------------------------------------- */
void printMessageAdded(SharedBuffer* this, int index, Message message) {
	printf("Action: New Message added:\n");
	printf("\tMessage index: %d\n", index);
	printf("\tLive Producers: %d\n", this->__producerCount);
	printf("\tLive Consumers: %d\n", this->__consumerCount);
	printf("\tMessage Info:\n");
	printf("\t\tKey: %d", message.key);
	printf("\t\tProducer ID: %d", message.producer_id);
	printf("\t\tDate Time: %s", ctime(&message.dateTime));
}
/* ---------------------------------------------------------------- */
void printMessageRead(SharedBuffer* this, int index, Message message) {
	printf("Action: Read Message:\n");
	printf("\tMessage index: %d\n", index);
	printf("\tLive Producers: %d\n", this->__producerCount);
	printf("\tLive Consumers: %d\n", this->__consumerCount);
	printf("\tMessage Info:\n");
	printf("\t\tKey: %d", message.key);
	printf("\t\tProducer ID: %d", message.producer_id);
	printf("\t\tDate Time: %s", ctime(&message.dateTime));
}
