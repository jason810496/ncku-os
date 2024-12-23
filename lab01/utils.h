#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "schema.h"

#ifndef UTILS_H
#define UTILS_H

// color definitions
extern const char* COLOR_RED;
extern const char* COLOR_BLUE;
extern const char* COLOR_GREEN;
extern const char* COLOR_RESET;
// verb enum
enum role { SENDER, RECEIVER };
// IPC method
enum ipc_method { MESSAGE_PASSING = 1, SHARED_MEMORY };
// message definition
extern char* EXIT_MESSAGE;
extern char* START_MESSAGE;
/*
    function prototypes
*/
// constructor
message_t* create_message(char* text);
mailbox_t* create_mailbox(enum ipc_method method, enum role cur_role);
timestamp_t* create_timestamp();
// destructor
void free_mailbox(mailbox_t* mailbox, enum role cur_role);

// message
int is_exit_message(message_t* message);
int is_start_message(message_t* message);
int is_empty_message(message_t* message);

// stdout
void print_with_color(const char* color, const char* message);
void show_time(enum role cur_role, double elapsed_time);
void show_communication_method(enum ipc_method method);
void update_elapsed_time(struct timespec start_time, struct timespec end_time,
                         double* elapsed_time);
void get_clock_time(struct timespec* time);

#endif