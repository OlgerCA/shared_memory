
#include <stdio.h>
#include <Message.h>

int main (int argc, char *argv[])
{
    puts("This is the consumer process");

    // Code in common project
    Message m;
    m.key = 10;
    message_print(&m);
    return 0;

    // TODO consumer: get command-line arguments: buffer (or file name) and mean waiting time.

    // TODO consumer: use mmap to get a pointer to a SharedBuffer using the provided file name

    // TODO consumer: increase consumer counter inside SharedBuffer

    // TODO consumer: get semaphore instance using the provided name inside SharedBuffer

    // TODO consumer: read messages until a termination message is read or p_id % 5 = message_key, using the provided waiting time. Print to the console each time a new message is read.

    // TODO consumer: decrease consumer counter inside SharedBuffer and munmap

    // TODO consumer: print report

}