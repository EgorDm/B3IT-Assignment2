//
// Created by egordm on 16-2-2018.
//

#ifndef INTERTECH_CUSTOM_TASKS_H
#define INTERTECH_CUSTOM_TASKS_H

#include "scheduler.h"

namespace internals {
    /**
     * Hehehehe such a pun
     * Basically storing a pointer to a function and executing when its time. Use as a delay function.
     */
    class AppointedTask : public Task {
    private:
        bool (*task_f)();
    public:
        explicit AppointedTask(bool (*task_f)()) : task_f(task_f) {}

        bool execute() override { return (*task_f)();  }
    };

    /**
     * Same as the above task but for a routine
     * TODO: add a function to tell if routine still should be running
     */
    class AppointedRoutine : public RoutineTask {
    private:
        bool (*task_f)();
    public:
        explicit AppointedRoutine(unsigned long interval, bool (*task_f)()) : RoutineTask(interval), task_f(task_f) {}

        bool execute() override { return (*task_f)();  }
    };
}

#endif //INTERTECH_CUSTOM_TASKS_H
