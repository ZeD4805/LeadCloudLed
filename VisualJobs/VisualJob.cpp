//
// Created by Asus on 04/11/2021.
//

#include "VisualJob.h"

virtual void VisualJob::start(){}

virtual void VisualJob::run(){}

bool VisualJob::comparableTo(VisualJob v){
    if(this->config->ID != "" && !v.config->ID.compare(v.ID)){
        return false;
    }

    if(jobType != JobTypeUndefined && jobType != v.jobType)
        return false;

    return true;
}
