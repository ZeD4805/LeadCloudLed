//
// Created by Asus on 04/11/2021.
//

#include "JobScheduler.h"

JobScheduler::JobScheduler() {
    //fallbackJob = new VisualJob(); //to be replaced by actual job
}

JobScheduler::~JobScheduler(){
    //delete(fallbackJob);
}

void JobScheduler::addJob(VisualJob* j){
    switch (j->priorityLevel) {
        case PriorityLevel::Emergency:
            emergencyJobs.insertTail(j);
            break;
        case PriorityLevel::Raised:
            raisedJobs.insertHead(j);//add in front so it happens right as it arrives
            break;
        case PriorityLevel::Base:
            baseJobs.insertTail(j);
            break;
        default:
            break;
            //we don't want an undefined case in the job queue
    }
}

VisualJob* JobScheduler::removeJob(string jobId){
    VisualJob j = {
            jobID,
            JobTypeUndefined,
            {0, 0},
            {0, 0},
            PriorityUndefined};

    T* removed = nullptr;

    switch (j->priorityLevel) {
        case PriorityLevel::Emergency:
            removed = emergencyJobs.removeThroughComparable(j);
            break;
        case PriorityLevel::Raised:
            removed = raisedJobs.removeThroughComparable(j);;
            break;
        case PriorityLevel::Base:
            removed = baseJobs.removeThroughComparable(j);;
            break;
        default:
            break;
    }

    return removed;
}


void JobScheduler::scheduleJob(){
    //save current job
    switch (currJob->priorityLevel) {
        case Emergency:
            emergencyJobs.insertTail(currJob);
            break;
        case Raised:
            currJob->priorityLevel = Base; //remove raised status
            baseJobs.insertTail(currJob);
            break;
        case Base:
            baseJobs.insertTail(currJob);
            break;
        default:
            break;
    }

    if(!emergencyJobs.isEmpty()){
        currJob = emergencyJobs.popHead();
    }
    else if(!raisedJobs.isEmpty()){
        currJob = raisedJobs.popHead();
    }
    else if(!baseJobs.isEmpty()){
        currJob = baseJobs.popHead();
    }
    /*else (){ //fallback option

    }*/
    //needs fallback option
    //e.g. text saying "no jobs queued :("


    //run job.start() if necessary
    if(currJob->state == uninitialized){
        currJob->start(); //shouldn't take a visible amount of time
    }

    while (){
        currJob->run();
    }

    return;

    //run job.run() until time
}