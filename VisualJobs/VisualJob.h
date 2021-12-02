//
// Created by Asus on 04/11/2021.
//

#ifndef LEAD_CLOUD_VISUALJOB_H
#define LEAD_CLOUD_VISUALJOB_H

#include <cstdint>
#include <ctime>
#include <string>
#include <utility>

#include "VisualJobConfig.h"
#include "utils/DoubleLinkedList.h"

using namespace std;

class VisualJob {
protected:
    VisualJobConfig* config;
    //as pointer so you can refer to inheriting class's config class
    //VisualJobConfig without replicating the variable

    JobState state = uninitialized;

    //for job lists only
    VisualJob* prev, *next;
public:
    virtual void start(); //setup, runs once
    virtual void run(); //function, runs continually in a loop for the time given

    bool comparableTo(VisualJob v);

    explicit VisualJob(VisualJobConfig* config) : config(config){
        state = uninitialized;
        prev = nullptr;
        next = nullptr;
    }

    friend class JobScheduler;
    friend class DoubleLinkedList<VisualJob>;
};


#endif //LEAD_CLOUD_VISUALJOB_H
