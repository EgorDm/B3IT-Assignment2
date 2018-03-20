//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_TASKS_H
#define B3ITASSIGNMENT2_TASKS_H


#include "../lib/scheduler/scheduler.h"

class WiFiConnectTimeout : internals::Task {
private:
    bool execute() override;

};


#endif //B3ITASSIGNMENT2_TASKS_H
