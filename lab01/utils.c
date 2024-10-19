#include "utils.h"

void print_with_color(const char* color, const char* message){
    printf("%s%s%s", color, message, COLOR_RESET);
}

void show_time(enum verb verb, struct timespec start, struct timespec end){
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) *1e-9;
    char *verb_str = (verb == SENDING) ? "sending" : "receiving";
    printf("Total time taken in %s msg: %f s\n", verb_str, elapsed_time);
}