#include "receiver.h"
#include "utils.h"
/*
Usage: ./receiver 1
(1 for Message Passing, 2 for Shared Memory)

Message size: 1-1024 bytes
*/

void receive(message_t* message_ptr, mailbox_t* mailbox_ptr, timestamp_t* timestamp_ptr){
    /*  TODO: 
        1. Use flag to determine the communication method
        2. According to the communication method, receive the message
    */
    enum ipc_method communication_method = (enum ipc_method)mailbox_ptr->flag;
    sem_wait(mailbox_ptr->semaphore_full);
    get_clock_time(&timestamp_ptr->start_time);
    if(communication_method == MESSAGE_PASSING){
        int msgid = msgget(mailbox_ptr->storage.msqid, 0666 | IPC_CREAT);
        if(msgid == -1){
            fprintf(stderr, "msgget failed\n");
            return;
        }
        if(msgrcv(msgid, message_ptr, sizeof(message_t), 0, 0) == -1){
            fprintf(stderr, "msgrcv failed\n");
            return;
        }
    }else if(communication_method == SHARED_MEMORY){
        memcpy(message_ptr, mailbox_ptr->storage.shared_memory_addr, sizeof(message_t));
    }
    else{
        fprintf(stderr, "Invalid communication method\n");
        return;
    }
    get_clock_time(&timestamp_ptr->end_time);
    sem_post(mailbox_ptr->semaphore_empty);
    update_elapsed_time(timestamp_ptr->start_time, timestamp_ptr->end_time, &timestamp_ptr->elapsed_time);
}

int main(int argc, char* argv[]){
    /*  TODO: 
        1) Call receive(&message, &mailbox) according to the flow in slide 4
        2) Measure the total receiving time
        3) Get the mechanism from command line arguments
            • e.g. ./receiver 1
        4) Print information on the console according to the output format
        5) If the exit message is received, print the total receiving time and terminate the receiver.c
    */
    timestamp_t* timestamp = create_timestamp();

    if(argc != 2){
        fprintf(stderr, "Usage: %s <num>\n", argv[0]);
        fprintf(stderr, "(1 for Message Passing, 2 for Shared Memory)\n");
        return EXIT_FAILURE;
    }
    int flag = atoi(argv[1]);
    enum ipc_method communication_method = (enum ipc_method)flag;
    show_communication_method(communication_method);

    mailbox_t* mailbox = create_mailbox(flag, RECEIVER);
    if(mailbox == NULL){
        fprintf(stderr, "Mailbox creation failed\n");
        return EXIT_FAILURE;
    }
    message_t* message = create_message("");
    // block, waiting for the start message
    receive(message, mailbox, timestamp);
    if(!is_start_message(message)){
        fprintf(stderr, "First message should be the start message\n");
        return EXIT_FAILURE;
    }
    

    receive(message, mailbox, timestamp);
    while(!is_exit_message(message)){
        print_with_color(COLOR_BLUE, "Receiving Message: ");
        printf("%s", message->text);

        receive(message, mailbox,timestamp);

    }
    print_with_color(COLOR_RED, "Sender exit!\n");

    show_time(RECEIVER, timestamp->elapsed_time);
    free_mailbox(mailbox, RECEIVER);

    return EXIT_SUCCESS;
}