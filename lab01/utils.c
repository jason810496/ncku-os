#include "utils.h"

const char *COLOR_RED = "\033[0;31m";
const char *COLOR_BLUE = "\033[0;34m";
const char *COLOR_RESET = "\033[0m";

char* EXIT_MESSAGE = "EXIT";
char* START_MESSAGE = "START";

void print_with_color(const char* color, const char* message){
    printf("%s%s%s", color, message, COLOR_RESET);
}

void update_elapsed_time(struct timespec start_time, struct timespec end_time, double* elapsed_time){
    *elapsed_time += (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) * 1e-9;
}

void get_clock_time(struct timespec* time){
    clock_gettime(CLOCK_MONOTONIC, time);
}

void show_time(enum verb verb, double elapsed_time){
    char *verb_str = (verb == SENDING) ? "sending" : "receiving";
    printf("Total time taken in %s msg: %f s\n", verb_str, elapsed_time);
}

int is_exit_message(message_t* message){
    return strcmp(message->text, EXIT_MESSAGE) == 0;
}

int is_start_message(message_t* message){
    return strcmp(message->text, START_MESSAGE) == 0;
}