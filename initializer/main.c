
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <SharedBuffer.h>

int main (int argc, char *argv[]) {

    puts("This is the initializer process");

    // Reading of arguments.
    char* buffer_name = NULL;
    int queue_size = 0;
    if (argc != 3) {
        fputs("Invalid number of arguments! Specify buffer name and queue size.", stderr);
        return -1;
    }
    buffer_name = argv[1];
    queue_size = atoi(argv[2]);
    if (queue_size <= 0) {
        fputs("Invalid queue size.", stderr);
        return -1;
    }

    // Create a new file that will be mapped by mmap.
    int fd = open(buffer_name, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if (fd == -1) {
        perror("Error opening file for writing");
        return -1;
    }
    __off_t offset = lseek(fd, sizeof(SharedBuffer) + sizeof(Message) * queue_size - 1, SEEK_SET);
    if (offset == -1) {
        close(fd);
        perror("Error calling lseek() to 'stretch' the file");
        return -1;
    }
    ssize_t result = write(fd, "", 1);
    if (result != 1) {
        close(fd);
        perror("Error writing last byte of the file");
        return -1;
    }

    // Creating a new named semaphore.
    char* semaphore_name = "/shared_sem";
    sem_t* semaphore = sem_open(semaphore_name, O_CREAT /*| O_EXCL*/, S_IRWXO, 0);
    if (semaphore == SEM_FAILED) {
        close(fd);
        perror("Error creating semaphore");
        return -1;
    }

    // Calling mmap to map file to memory.
    size_t map_size = (size_t) (offset + 1);
    SharedBuffer* buffer = (SharedBuffer*) mmap(NULL, map_size
            ,PROT_EXEC | PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
        close(fd);
        sem_close(semaphore);
        sem_unlink(semaphore_name);
        perror("Error mapping file");
        return -1;
    }

    // Initializing buffer.
    shared_buffer_init(buffer, queue_size, map_size, semaphore_name);

    // Freeing resources.
    munmap(buffer, map_size);
    close(fd);
    sem_close(semaphore);
    // This line should be moved to finalizer process later.
    sem_unlink(semaphore_name);

    return 0;
}