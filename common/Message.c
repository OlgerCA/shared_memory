#include <stdio.h>
#include "Message.h"

void message_print(Message *message) {
    printf("Message with key %d\n", message->key);
}
