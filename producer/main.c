
#include <stdio.h>
#include <Message.h>
#include <fcntl.h>
#include <SharedBuffer.h>
#include <sys/mman.h>
#include <unistd.h>
#include <math.h>
#include <Semaphore.h>
#include <time.h>
#include <stdlib.h>

char* getDateTime();

int main (int argc, char *argv[])
{
    printf("This is the producer process\n");
    time_t t;
    char* buffer_name = NULL;
    double mean = 1;
    int messagesProduced = 0;
    double waitTime = 0;
    double semWaitTime = 0;

    // TODO producer: get command-line arguments: buffer (or file name) and mean waiting time.
    if (argc != 3) {
        printf("Invalid number of arguments! Specify buffer name and mean wait time.");
        return -1;
    }
    buffer_name = argv[1];
    mean = strtod(argv[2], NULL);


    // TODO producer: use mmap to get a pointer to a SharedBuffer using the provided file name
    SharedBuffer* buffer = shared_buffer_get(buffer_name);

    // TODO producer: get semaphore instance using the provided name inside SharedBuffer

    sem_t *semaphore = get_shared_semaphore(buffer->__semaphoreName);
    if(!semaphore){
        perror("Error getting semaphore");
        return -1;
    }

    // TODO producer: increase producers counter inside SharedBuffer
    int producerId = shared_buffer_increase_producer_count(buffer, semaphore);

    srand((unsigned) time(&t));


    // TODO producer: create messages while SharedBuffer is active and using the provided waiting time. Print to the console each time a new message is created.
    while(1){
        double exp_rnd_wait;

        if(!buffer->__isActive){
            break;
        }
        //TODO: create message
        Message message;
        message.is_termination_message = 0;
        message.producer_id = producerId;
        message.key = rand() % 5;
        message.dateTime = getDateTime();

        //TODO: post message to queue
        semWaitTime += shared_buffer_put_message(buffer, message, semaphore);
        messagesProduced++;

        exp_rnd_wait = -log((double)rand() / (double)((unsigned)RAND_MAX + 1)) * mean;
        waitTime += exp_rnd_wait;
        sleep(exp_rnd_wait);

        // TODO producer: print report
        printf("Producer Summary:\n");
        printf("\tProduce ID:%d\n", producerId);
        printf("\tAccum wait time:%lf\n", waitTime);
        printf("\tAccum wait time blocked:%lf\n", semWaitTime);
        printf("\tetc...");


    }

    // TODO producer: decrease producers counter inside SharedBuffer and munmap
    shared_buffer_decrease_producer_count(buffer, semaphore);



    return 0;
}

char* getDateTime(){
    char *dateTime = NULL;
    time_t rawtime = NULL;
    time(&rawtime);
    dateTime = ctime(&rawtime);
    return dateTime;
}