#include "sender.h"
#include "utils.h"
/*
Usage: ./sender 1 input.txt
(1 for Message Passing, 2 for Shared Memory)

Message size: 1-1024 bytes
*/

void send(message_t *message_ptr, mailbox_t* mailbox_ptr){
    /*  TODO: 
        1. Use flag to determine the communication method
        2. According to the communication method, send the message
    */
    enum ipc_method communication_method = (enum ipc_method)mailbox_ptr->flag;
    if(communication_method == MESSAGE_PASSING){
        int msgid = msgget(mailbox_ptr->storage.msqid, 0666 | IPC_CREAT);
        if(msgid == -1){
            fprintf(stderr, "msgget failed\n");
            return;
        }
        if(msgsnd(msgid, message_ptr, sizeof(message_t), 0) == -1){
            fprintf(stderr, "msgsnd failed\n");
            return;
        }
    }else if(communication_method == SHARED_MEMORY){
        sem_wait(mailbox_ptr->semaphore_empty);
        memcpy(mailbox_ptr->storage.shared_memory_addr, message_ptr, sizeof(message_t));
        sem_post(mailbox_ptr->semaphore_full);
    }
    else{
        fprintf(stderr, "Invalid communication method\n");
        return;
    }
}

int main(int argc, char* argv[]){
    /*  TODO: 
        1) Call send(message, &mailbox) according to the flow in slide 4
        2) Measure the total sending time
        3) Get the mechanism and the input file from command line arguments
            • e.g. ./sender 1 input.txt
                    (1 for Message Passing, 2 for Shared Memory)
        4) Get the messages to be sent from the input file
        5) Print information on the console according to the output format
        6) If the message form the input file is EOF, send an exit message to the receiver.c
        7) Print the total sending time and terminate the sender.c
    */
    struct timespec start_time, end_time;
    double elapsed_time = 0.0;

    if(argc != 3){
        fprintf(stderr, "Usage: %s <num> input.txt\n", argv[0]);
        fprintf(stderr, "(1 for Message Passing, 2 for Shared Memory)\n");
        return EXIT_FAILURE;
    }
    int flag = atoi(argv[1]);
    enum ipc_method communication_method = (enum ipc_method)flag;
    show_communication_method(communication_method);

    char* input_file = argv[2];
    FILE* fp = fopen(input_file, "r");
    if(fp == NULL){
        fprintf(stderr, "File open error: %s\n", input_file);
        return EXIT_FAILURE;
    }

    mailbox_t* mailbox = create_mailbox(flag, SENDER);
    if(mailbox == NULL){
        fprintf(stderr, "Mailbox creation failed\n");
        return EXIT_FAILURE;
    }

    message_t* start_message = create_message(START_MESSAGE);
    send(start_message, mailbox);
    free(start_message);

    char buffer[1024];
    while(fgets(buffer, 1024, fp) != NULL){
        print_with_color(COLOR_BLUE, "Sending message: ");
        printf("%s", buffer);
        message_t* message = create_message(buffer);

        get_clock_time(&start_time);
        send(message, mailbox);
        get_clock_time(&end_time);
        update_elapsed_time(start_time, end_time, &elapsed_time);
        

        free(message);
    }
    fclose(fp);
    print_with_color(COLOR_RED, "End of input file !\n");

    message_t* exit_message = create_message(EXIT_MESSAGE);
    send(exit_message, mailbox);
    free(exit_message);
    print_with_color(COLOR_RED, "exit!\n");

    show_time(SENDER, elapsed_time);
    free_mailbox(mailbox, SENDER);

    return EXIT_SUCCESS;
}