
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <SharedBuffer.h>
#include <Semaphore.h>

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

    // Creating a new named buffer_sem_name.
    sem_t* buffer_sem = create_shared_semaphore(buffer_sem_name, 1);
    if(!buffer_sem){
        perror("Error creating buffer_sem_name");
        return -1;
    }

    // Creating a new named empty_sem_name.
    sem_t* empty_sem = create_shared_semaphore(empty_sem_name, queue_size - 1);
    if(!empty_sem){
        perror("Error creating buffer_sem_name");
        return -1;
    }

    // Creating a new named fill_sem_name.
    sem_t* fill_sem = create_shared_semaphore(fill_sem_name, 0);
    if(!fill_sem){
        perror("Error creating buffer_sem_name");
        return -1;
    }

    // Calling mmap to map file to memory.
    size_t map_size = (size_t) (offset + 1);
    SharedBuffer* buffer = (SharedBuffer*) mmap(NULL, map_size
            ,PROT_EXEC | PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
        close(fd);
        sem_close(buffer_sem);
        sem_unlink(buffer_sem_name);
        perror("Error mapping file");
        return -1;
    }

    // Initializing buffer.
    shared_buffer_init(buffer, queue_size, map_size, buffer_sem_name, fill_sem_name, empty_sem_name);

    return 0;
}