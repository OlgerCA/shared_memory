
#include <stdio.h>
#include <Message.h>

int main (int argc, char *argv[])
{
    puts("This is the finalizer process");

    // Code in common project
    Message m;
    m.key = 10;
    message_print(&m);

    // TODO finalizer: get command-line argument: buffer (or file name)

    // TODO finalizer: use mmap to get a pointer to a SharedBuffer using the provided file name

    // TODO finalizer: call shared_buffer_set_inactive, and wait until there are no producers alive

    // TODO finalizer: send one finalization message for each consumer that is alive and wait until all are finished

    // TODO finalizer: munmap and release buffer (delete shared file)

    // TODO finalizer: print report

    return 0;
}