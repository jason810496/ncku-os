#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>

#ifndef SCHEMA_H
#define SCHEMA_H

typedef struct {
    int flag;      // 1 for message passing, 2 for shared memory
    union{
        key_t msqid; //for system V api. You can replace it with struecture for POSIX api
        char* shared_memory_addr;
    }storage;
} mailbox_t;


typedef struct {
    char text[1024];
} message_t;

#endif