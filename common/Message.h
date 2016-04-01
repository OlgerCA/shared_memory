
#ifndef SHARED_MEMORY_MESSAGE_H
#define SHARED_MEMORY_MESSAGE_H

typedef struct {
    int key;
} Message;

void message_print(Message* message);

#endif
