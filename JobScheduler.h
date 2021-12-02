//
// Created by Asus on 04/11/2021.
//

#ifndef LEAD_CLOUD_JOBSCHEDULER_H
#define LEAD_CLOUD_JOBSCHEDULER_H

#include "VisualJobs/VisualJob.h"
#include "utils/DoubleLinkedList.h"

using namespace std;

class JobScheduler {
private:
    DoubleLinkedList<VisualJob> emergencyJobs;
    DoubleLinkedList<VisualJob> raisedJobs;
    DoubleLinkedList<VisualJob> baseJobs;

    VisualJob* currJob = nullptr;
    //VisualJob* fallbackJob;
public:
    JobScheduler();
    ~JobScheduler();

    void addJob(VisualJob* j);
    VisualJob* removeJob(string ID);

    void scheduleJob();
};


#endif //LEAD_CLOUD_JOBSCHEDULER_H
