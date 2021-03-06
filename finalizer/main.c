/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * Finalizer
 * Copyright (C) 2016 Wil C <wilz04@gmail.com>
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <SharedBuffer.h>
#include <fcntl.h>
#include "Semaphore.h"

int main (int argc, char* argv[]) {
	puts("This is the finalizer process.\n");
	
	if (argc != 3) {
		fputs("Invalid number of arguments! Specify buffer name and queue size.", stderr);
		return -1;
	}
	
	// TODO finalizer: get command-line argument: buffer (or file name)
	char* buffer_name = argv[1];
	int queue_size = atoi(argv[2]);
	if (queue_size <= 0) {
		fputs("Invalid queue size.", stderr);
		return -1;
	}
	
	//SharedBuffer* buffer = shared_buffer_get(buffer_name);
	int fd = shared_buffer_open_file(buffer_name);
	if (fd == -1) {
		perror("Error opening file for writing");
		exit(-1);
	}
	
	size_t map_size = shared_buffer_get_size(fd, queue_size);
	if (map_size == -1) {
		shared_buffer_close_file(fd);
		perror("Error calling lseek() to 'stretch' the file");
		return -1;
	}
	
	SharedBuffer* buffer = shared_buffer_load(fd, map_size);
	if (buffer == MAP_FAILED) {
		shared_buffer_unlink(buffer, fd, map_size);
		perror("Error mapping file");
		return -1;
	}
	
	sem_t* sem_buff = get_shared_semaphore(buffer_sem_name);
	if (sem_buff == SEM_FAILED) {
		shared_buffer_unlink(buffer, fd, map_size);
		perror("Error getting semaphore sem_buff");
		return -1;
	}
	
	sem_t* sem_fill = get_shared_semaphore(fill_sem_name);
	if (sem_fill == SEM_FAILED) {
		shared_buffer_unlink(buffer, fd, map_size);
		perror("Error getting semaphore sem_fill");
		return -1;
	}
	
	sem_t* sem_empt = get_shared_semaphore(empty_sem_name);
	if (sem_empt == SEM_FAILED) {
		shared_buffer_unlink(buffer, fd, map_size);
		perror("Error getting semaphore sem_empt");
		return -1;
	}
	
	sem_wait(sem_buff);
	
	// TODO finalizer: call shared_buffer_set_inactive, and wait until there are no producers alive
	shared_buffer_set_inactive(buffer);
	
	int post_mutex = 0;
	for(; post_mutex < queue_size; post_mutex++){
		sem_post(sem_fill);
		sem_post(sem_empt);
	}

	int id = shared_buffer_get_producer_count(buffer);
	printf("Stopping the %d producers...\n", id);
	do {
		sem_post(sem_buff);
		sleep(2);
		sem_wait(sem_buff);
	} while (
		shared_buffer_get_producer_count(buffer) > 0
	);
	puts("All producers stopped.\n");
	
	// TODO finalizer: send one finalization message for each consumer that is alive and wait until all are finished
	id = shared_buffer_get_consumer_count(buffer);
	printf("Stopping the %d consumers...\n", id);
	for (; id > 0; id--) {
		printf("Stopping consumer %d...\n", id);
		shared_buffer_put_stop(buffer, id);
	}
	
	do {
		sem_post(sem_buff);
		sleep(2);
		sem_wait(sem_buff);
	} while (
		shared_buffer_get_consumer_count(buffer) > 0
	);
	puts("All consumers stopped.\n");
	
	// TODO finalizer: print report
	printf("Finalizer Summary:\n");
	printf("\tTotal messages produced: %d\n", buffer->__messageCount);
	printf("\tTotal messages read: %d\n", buffer->__readCount);
	
	// TODO finalizer: munmap and release buffer (delete shared file)
	shared_buffer_unlink(buffer, fd, map_size);
	sem_close(sem_buff);
	sem_unlink(buffer_sem_name);
	sem_close(sem_fill);
	sem_unlink(fill_sem_name);
	sem_close(sem_empt);
	sem_unlink(empty_sem_name);
	
	return 0;
}
