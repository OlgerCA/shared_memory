
#include <stdio.h>
#include <Message.h>
#include <SharedBuffer.h>
#include <unistd.h>
#include <math.h>
#include <Semaphore.h>
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

    // TODO producer: get buffer_sem instance using the provided name inside SharedBuffer

    sem_t *buffer_sem = get_shared_semaphore(buffer->__buffer_sem_name);
    if(!buffer_sem){
        perror("Error getting buffer_sem");
        return -1;
    }

    sem_t *fill_sem = get_shared_semaphore(buffer->__fill_sem_name);
    if(!fill_sem){
        perror("Error getting buffer_sem");
        return -1;
    }

    sem_t *empty_sem = get_shared_semaphore(buffer->__empty_sem_name);
    if(!empty_sem){
        perror("Error getting buffer_sem");
        return -1;
    }

    // TODO producer: increase producers counter inside SharedBuffer
    int producerId = shared_buffer_increase_producer_count(buffer, buffer_sem);

    srand((unsigned) time(&t));


    // TODO producer: create messages while SharedBuffer is active and using the provided waiting time. Print to the console each time a new message is created.
    while(1){
        printf("buffer full! sleeping...\n");
        clock_t start = clock(), diff;
        sem_wait(empty_sem);
        diff = clock() - start;
        printf("waking up! need to produce!\n");
        semWaitTime += diff;

        double exp_rnd_wait;

        if(!buffer->__isActive){
            break;
        }
        //TODO: create message
        Message message;
        message.is_termination_message = 0;
        message.producer_id = producerId;
        message.key = rand() % 5;
        message.readFlag = false;
        time(&message.dateTime);

        //TODO: post message to queue
        semWaitTime += shared_buffer_put_message(buffer, message, buffer_sem);
        messagesProduced++;

        exp_rnd_wait = -log((double)rand() / (double)((unsigned)RAND_MAX + 1)) * mean;
        waitTime += exp_rnd_wait;
        sem_post(fill_sem);
        printf("sleeping %lf seconds\n", exp_rnd_wait);
        sleep(exp_rnd_wait);
    }

    // TODO producer: decrease producers counter inside SharedBuffer and munmap
    shared_buffer_decrease_producer_count(buffer, buffer_sem);

    // TODO producer: print report
    printf("Producer Summary:\n");
    printf("\tProducer ID:%d\n", producerId);
    printf("\tMessages produced: %d\n", messagesProduced);
    printf("\tAccum wait time:%lf\n", waitTime);
    printf("\tAccum wait time blocked:%lf\n", semWaitTime);
    printf("\tetc...");

    return 0;
}