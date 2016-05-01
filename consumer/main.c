
#include <stdio.h>
#include <Message.h>
#include <SharedBuffer.h>
#include <stdlib.h>
#include <math.h>
#include <Semaphore.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    printf("This is the consumer process\n");

    char* buffer_name = NULL;
    double mean = 1;
    int messagesRead = 0;
    double waitTime = 0;
    double semWaitTime = 0;


    if (argc != 3) {
        printf("Invalid number of arguments! Specify buffer name and mean wait time.");
        return -1;
    }
    buffer_name = argv[1];
    mean = strtod(argv[2], NULL);


    // TODO consumer: use mmap to get a pointer to a SharedBuffer using the provided file name
    SharedBuffer* buffer = shared_buffer_get(buffer_name);


    // TODO consumer: get semaphore instance using the provided name inside SharedBuffer
    sem_t *semaphore = get_shared_semaphore(buffer->__buffer_sem_name);
    if(!semaphore){
        perror("Error getting semaphore");
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

    // TODO consumer: increase producers counter inside SharedBuffer
    int consumerId = shared_buffer_increase_consumer_count(buffer, semaphore);


    // TODO consumer: read messages until a termination message is read or p_id % 5 = message_key, using the provided waiting time. Print to the console each time a new message is read.
    while(1){
        printf("sleeping until buffer not empty...\n");
        clock_t start = clock(), diff;
        sem_wait(fill_sem);
        diff = clock() - start;
        printf("waking up! buffer not empty!\n");
        semWaitTime += diff;

        double exp_rnd_wait;

        Message message = shared_buffer_get_message(buffer, semaphore);
        if(consumerId % 5 == message.key) {
            sem_post(empty_sem);
            break;
        }
        if(message.key != -1) messagesRead++;

        semWaitTime += message.semWaitTime;

        exp_rnd_wait = -log((double)rand() / (double)((unsigned)RAND_MAX + 1)) * mean;
        waitTime += exp_rnd_wait;
        sem_post(empty_sem);
        printf("sleeping %lf seconds\n", exp_rnd_wait);
        sleep(exp_rnd_wait);
    }

    // TODO consumer: decrease producers counter inside SharedBuffer and munmap
    shared_buffer_decrease_consumer_count(buffer, semaphore);

    // TODO consumer: print report
    printf("Consumer Summary:\n");
    printf("\tConsumer ID: %d\n", consumerId);
    printf("\tTotal reads: %d\n", messagesRead);
    printf("\tAccum wait time: %lf\n", waitTime);
    printf("\tAccum wait time blocked: %lf\n", semWaitTime);

    return 0;
}
