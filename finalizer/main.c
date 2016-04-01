
#include <stdio.h>
#include <Message.h>

int main (int argc, char *argv[])
{
    puts("This is the finalizer process");

    // Code in common project
    Message m;
    m.key = 10;
    message_print(&m);
    return 0;
}