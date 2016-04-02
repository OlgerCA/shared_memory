
#include <stdio.h>
#include <Message.h>

int main (int argc, char *argv[])
{
    puts("This is the initializer process");

    // Code in common project
    Message m;
    m.key = 10;
    message_print(&m);

    // TODO initializer: get command-line arguments: buffer (or file name) and queue size.

    // TODO initializer: create a new mapping file of size = (sizeof(SharedBuffer) - sizeof(Message*) + sizeof(Message) * queueSize). Use the provided file name.

    // TODO initializer: create a new named semaphore

    // TODO initializer: use mmap to get a pointer to a SharedBuffer using the provided file name

    // TODO initializer: initialize SharedBuffer and munmap

    // Not sure if process needs to be kept alive

    return 0;
}