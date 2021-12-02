//
// Created by zed4805 on 08/11/21.
//

#ifndef LEAD_CLOUD_TICKET_H
#define LEAD_CLOUD_TICKET_H

#include <string>
#include <utility>

using namespace std;

class Ticket {
public:
    int ticketNumber;

    explicit Ticket(int ticketNumber) :
            ticketNumber(ticketNumber){}

};


#endif //LEAD_CLOUD_TICKET_H
