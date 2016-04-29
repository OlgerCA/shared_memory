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
	puts("This is the finalizer process.");
	
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
		return -1;
	}

	sem_t* buffer_sem = get_shared_semaphore(buffer_sem_name);
	if (buffer_sem == SEM_FAILED) {
		perror("Error creating semaphore");
		return -1;
	}

	sem_t* fill_sem = get_shared_semaphore(fill_sem_name);
	if (fill_sem == SEM_FAILED) {
		perror("Error creating semaphore");
		return -1;
	}

	sem_t* empty_sem = get_shared_semaphore(empty_sem_name);
	if (empty_sem == SEM_FAILED) {
		perror("Error creating semaphore");
		return -1;
	}

	sem_wait(buffer_sem);

	// TODO finalizer: call shared_buffer_set_inactive, and wait until there are no producers alive
	shared_buffer_set_inactive(buffer);
    int post_mutex = 0;
    for(; post_mutex < queue_size; post_mutex++){
        sem_post(fill_sem);
        sem_post(empty_sem);
    }
	
	do {
		sem_post(buffer_sem);
		sleep(2);
		sem_wait(buffer_sem);
	} while (
		shared_buffer_get_producer_count(buffer) > 0
	);

	// TODO finalizer: send one finalization message for each consumer that is alive and wait until all are finished
	int id = shared_buffer_get_consumer_count(buffer);
	for (; id > 0; id--) {
		shared_buffer_put_stop(buffer, id);
	}

	do {
		sem_post(buffer_sem);
		sleep(2);
		sem_wait(buffer_sem);
	} while (
		shared_buffer_get_consumer_count(buffer) > 0
	);



	// TODO finalizer: munmap and release buffer (delete shared file)
	shared_buffer_unlink(buffer, fd, map_size);
	sem_close(buffer_sem);
	sem_unlink(buffer_sem_name);
    sem_close(fill_sem);
    sem_unlink(fill_sem_name);
    sem_close(empty_sem);
    sem_unlink(empty_sem_name);

	// TODO finalizer: print report

	return 0;
}
