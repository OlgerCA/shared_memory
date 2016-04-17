
#ifndef SHARED_MEMORY_MESSAGE_H
#define SHARED_MEMORY_MESSAGE_H

#include <time.h>

#define bool int
#define true 1
#define false 0

typedef struct {
    int producer_id;
    bool is_termination_message;
    time_t dateTime;
    int key;
    int readFlag;
    double semWaitTime;
} Message;

void message_print(Message* message);

#endif
