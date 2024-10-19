#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "schema.h"

#ifndef UTILS_H
#define UTILS_H

// color definitions
extern const char *COLOR_RED;
extern const char *COLOR_BLUE;
extern const char *COLOR_RESET;
// verb enum
enum verb {
    SENDING,
    RECEIVING
};
// IPC method
enum ipc_method {
    MESSAGE_PASSING = 1,
    SHARED_MEMORY
};
// message definition
extern char* EXIT_MESSAGE;
extern char* START_MESSAGE;
/*
    function prototypes
*/
// message
int is_exit_message(message_t* message);
int is_start_message(message_t* message);

// stdout
void print_with_color(const char* color, const char* message);
void show_time(enum verb verb, double elapsed_time);
void update_elapsed_time(struct timespec start_time, struct timespec end_time, double* elapsed_time);
void get_clock_time(struct timespec* time);

#endif