#include <stdio.h>
#include <time.h>

// color definitions
const char COLOR_RED[] = "\033[0;31m";
const char COLOR_BLUE[] = "\033[0;34m";
const char COLOR_RESET[] = "\033[0m";
// verb enum
enum verb {
    SENDING,
    RECEIVING
};
// function prototypes
void print_with_color(const char* color, const char* message);
void show_time(enum verb verb, struct timespec start, struct timespec end);