
#ifndef SHARED_MEMORY_MESSAGE_H
#define SHARED_MEMORY_MESSAGE_H

#define bool int
#define true 1
#define false 0

typedef struct {
    int producer_id;
    bool is_termination_message;
    char* dateTime;
    int key;
} Message;

void message_print(Message* message);

#endif
