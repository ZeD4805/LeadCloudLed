//
// Created by zed4805 on 08/11/21.
//

#include <iostream>
#include "TicketQueue.h"

void TicketQueue::start(){

}

void TicketQueue::run(){

}

TicketQueue::TicketQueue(TicketQueueConfig config):
        VisualJob(&config.vjc){
    lastTicketNumber = -1;
}

void TicketQueue::newTicket(int newTicketNumber) {
    lastTicketNumber = newTicketNumber;
}
