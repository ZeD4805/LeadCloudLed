//
// Created by zed4805 on 08/11/21.
//

#ifndef LEAD_CLOUD_VISUALJOBCONFIG_H
#define LEAD_CLOUD_VISUALJOBCONFIG_H

#include <string>

using namespace std;

enum PriorityLevel{
    Emergency, //highest priority
    Raised, //temporarily raised priority (e.g. updated queue ticket)
    Base, //lowest priority

    PriorityUndefined // for search purposes
};

enum JobState{
    uninitialized, //hasn't run start()
    enabled, //has run start(), running loop()
    disabled,

    JobStateUndefined // for search purposes
};

enum JobType{
    info,
    alert,
    queue,
    custom, //unimplemented

    JobTypeUndefined //for search purposes
};

class VisualJobConfig {
public:
    string ID;

    JobType jobType;

    timespec runningTime; //how long it runs
    timespec updatePeriod; //if not specified, might depend on job type
    // {0, 0} -> update asap
    // {-1 , -1} -> never update
    // any other value, update after this time

    PriorityLevel priorityLevel;


    VisualJobConfig(string ID,
                    JobType jobType,
                    timespec runningTime,
                    PriorityLevel priorityLevel=Base,
                    timespec updatePeriod = {0, 0});

};
#endif //LEAD_CLOUD_VISUALJOBCONFIG_H
