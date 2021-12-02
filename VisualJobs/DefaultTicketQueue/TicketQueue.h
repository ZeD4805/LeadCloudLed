//
// Created by zed4805 on 08/11/21.
//

#ifndef LEAD_CLOUD_TICKETQUEUE_H
#define LEAD_CLOUD_TICKETQUEUE_H

#include "../VisualJob.h"
#include "Ticket.h"
#include "TicketQueueConfig.h"

#include <map>

using namespace std;

class TicketQueue: public VisualJob{
private:
    int lastTicketNumber;
public:
    explicit TicketQueue(TicketQueueConfig config);

    void start() override;
    void run() override;

    void newTicket(int newTicketNumber);
};


#endif //LEAD_CLOUD_TICKETQUEUE_H
