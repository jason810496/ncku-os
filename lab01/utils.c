#include "utils.h"

const char *COLOR_RED = "\033[0;31m";
const char *COLOR_BLUE = "\033[0;34m";\
const char *COLOR_GREEN = "\033[0;32m";
const char *COLOR_RESET = "\033[0m";

const char *KEY_FILE_NAME = "keyfile.tmp";
const char *SHARED_MEMORY_NAME = "shared_memory";

char* EXIT_MESSAGE = "EXIT";
char* START_MESSAGE = "START";

message_t* create_message(char* text){
    message_t* message = (message_t*)malloc(sizeof(message_t));
    strcpy(message->text, text);
    return message;
}

mailbox_t* create_mailbox(enum ipc_method method){
    mailbox_t* mailbox = (mailbox_t*)malloc(sizeof(mailbox_t));
    mailbox->flag = (int)method;
    if(method == MESSAGE_PASSING){
        key_t key = ftok(KEY_FILE_NAME, 666);
        mailbox->storage.msqid = key;
    }
    else if(method == SHARED_MEMORY){
        int shm_id = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if( ftruncate(shm_id, sizeof(message_t)) == -1){
            perror("ftruncate");
            exit(EXIT_FAILURE);
        }
        mailbox->storage.shared_memory_addr = mmap(NULL, sizeof(message_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
        if(mailbox->storage.shared_memory_addr == MAP_FAILED){
            perror("mmap");
            exit(EXIT_FAILURE);
        }
    }
    else{
        return NULL;
    }
    return mailbox;
}

void free_mailbox(mailbox_t* mailbox){
    if(mailbox->flag == SHARED_MEMORY){
        if(munmap(mailbox->storage.shared_memory_addr, sizeof(message_t)) == -1){
            perror("munmap");
            exit(EXIT_FAILURE);
        }
    }
    free(mailbox);
}

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

void show_communication_method(enum ipc_method method){
    char *method_str = (method == MESSAGE_PASSING) ? "Message Passing\n" : "Shared Memory\n";
    print_with_color(COLOR_GREEN, method_str);
}

int is_exit_message(message_t* message){
    return strcmp(message->text, EXIT_MESSAGE) == 0;
}

int is_start_message(message_t* message){
    return strcmp(message->text, START_MESSAGE) == 0;
}