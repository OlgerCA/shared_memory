
#include <stdio.h>
#include <Message.h>

int main (int argc, char *argv[])
{
    puts("This is the producer process");

    // Code in common project
    Message m;
    m.key = 10;
    message_print(&m);

    // TODO producer: get command-line arguments using optget: buffer (or file name) and mean waiting time.

    // TODO producer: use mmap to get a pointer to a SharedBuffer using the provided file name

    // TODO producer: increase producers counter inside SharedBuffer

    // TODO producer: get semaphore instance using the provided name inside SharedBuffer

    // TODO producer: create messages while SharedBuffer is active and using the provided waiting time. Print to the console each time a new message is created.

    // TODO producer: decrease producers counter inside SharedBuffer and munmap

    // TODO producer: print report

    return 0;
}