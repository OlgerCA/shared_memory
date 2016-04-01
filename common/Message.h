
#ifndef SHARED_MEMORY_MESSAGE_H
#define SHARED_MEMORY_MESSAGE_H

#define bool int
#define true 1
#define false 0

typedef struct {
    int producer_id;
    bool is_termination_message;
    // TODO common: add an appropriate time and date representation
    int key;
} Message;

void message_print(Message* message);

#endif
