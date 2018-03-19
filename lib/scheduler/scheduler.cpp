//
// Created by egordm on 14-2-18.
//

#include "scheduler.h"

using namespace internals;

Scheduler::~Scheduler() {
    for (auto &i : queue) delete i;
}

void Scheduler::tick(unsigned long time) {
    for(unsigned int i = 0; i < QUEUE_SIZE; ++i) {
        if(!queue[i] || !queue[i]->should_run(time)) continue;
        if(queue[i]->execute()) remove_task(i);
    }
}

int Scheduler::add_task(Task *task) {
    for (unsigned int i = 0; i < QUEUE_SIZE; ++i) {
        if (queue[i]) continue;
        queue[i] = task;
        return i;
    }
    return -1;
}

void Scheduler::remove_task(unsigned int i) {
    if (!queue[i]) return;
    delete queue[i];
    queue[i] = nullptr;
}
