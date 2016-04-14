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

#define SEM_NAME "/shared_sem"

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

	sem_t* sem = shared_buffer_open_semaphore(SEM_NAME);
	if (sem == SEM_FAILED) {
		perror("Error creating semaphore");
		return -1;
	}

	sem_wait(sem);
	
	// TODO finalizer: use mmap to get a pointer to a SharedBuffer using the provided file name
	int fd = shared_buffer_open_file(buffer_name);
	if (fd == -1) {
		perror("Error opening file for writing");
		return -1;
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

	// TODO finalizer: call shared_buffer_set_inactive, and wait until there are no producers alive
	shared_buffer_set_inactive(buffer);
	
	do {
		sem_post(sem);
		sleep(2);
		sem_wait(sem);
	} while (
		shared_buffer_get_producer_count(buffer) > 0
	);

	// TODO finalizer: send one finalization message for each consumer that is alive and wait until all are finished
	int id = shared_buffer_get_consumer_count(buffer);
	for (; id > 0; id--) {
		shared_buffer_put_stop(buffer, id);
	}

	do {
		sem_post(sem);
		sleep(2);
		sem_wait(sem);
	} while (
		shared_buffer_get_consumer_count(buffer) > 0
	);

	// TODO finalizer: munmap and release buffer (delete shared file)
	shared_buffer_unlink(buffer, fd, map_size);
	sem_close(sem);
	sem_unlink(SEM_NAME);

	// TODO finalizer: print report

	return 0;
}
