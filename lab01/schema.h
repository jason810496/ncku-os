#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SCHEMA_H
#define SCHEMA_H

typedef struct {
    int flag;      // 1 for message passing, 2 for shared memory
    union{
        int msqid; //for system V api. You can replace it with struecture for POSIX api
        char* shared_memory_addr;
    }storage;
} mailbox_t;


typedef struct {
    char text[1024];
} message_t;

// function prototypes
message_t* create_message(char* text);
mailbox_t* create_mailbox(int flag);

#endif