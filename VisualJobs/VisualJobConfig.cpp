//
// Created by zed4805 on 08/11/21.
//

#include "VisualJobConfig.h"

using namespace std;

VisualJobConfig::VisualJobConfig(
        string ID,
        JobType jobType,
        timespec runningTime,
        PriorityLevel priorityLevel,
        timespec updatePeriod):

        ID(std::move(ID)),
        jobType(jobType),
        runningTime(runningTime),
        priorityLevel(priorityLevel),
        updatePeriod(updatePeriod){

}