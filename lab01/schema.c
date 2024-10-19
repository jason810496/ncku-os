#include "schema.h"

message_t* create_message(char* text){
    message_t* message = (message_t*)malloc(sizeof(message_t));
    strcpy(message->text, text);
    return message;
}

mailbox_t* create_mailbox(int flag){
    mailbox_t* mailbox = (mailbox_t*)malloc(sizeof(mailbox_t));
    mailbox->flag = flag;
    return mailbox;
}