#include <iostream>

#include "JobScheduler.h"

int main() {
    JobScheduler scheduler;
    auto* j = new VisualJob("generic_hello", JobType::info, {5, 0}, Base,{0,0});

    scheduler.addJob(j);
    scheduler.scheduleJob();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}

