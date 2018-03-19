//
// Created by egordm on 14-2-18.
//

#ifndef INTER_TECH_SCHED_H
#define INTER_TECH_SCHED_H

#define QUEUE_SIZE 10

// Bitmask
#define BASE_TASK_TYPE 1
#define ROUTINE_TYPE 2

namespace internals {

    class Task {
    public:
        virtual ~Task() = default;

        /**
         * Execute. Just like isi...
         * You should return true if you want the task to stop
         * @return should stop the task?
         */
        virtual bool execute() { return true; };

        /**
        * Determine if the routine should be executed.
        * @param time
        * @return
        */
        virtual bool should_run(unsigned long time) { return true; };

        /**
         * Task type. Keep at default unless you override scheduler
         * @return
         */
        virtual char type() { return BASE_TASK_TYPE; }
    };

    class RoutineTask : public Task {
    protected:
        unsigned long last_run = 0;
        unsigned long interval;
    public:
        explicit RoutineTask(unsigned long interval) : interval(interval) {}

        ~RoutineTask() override = default;

        bool should_run(unsigned long time) override {
            if(last_run > time) last_run = 0; // Failsafe for millis rollover
            if (time - last_run < interval) return false;
            last_run = time;
            return true;
        }

        char type() override { return ROUTINE_TYPE; }
    };

    class Scheduler {
    protected:
        Task *queue[QUEUE_SIZE];

    public:
        Scheduler() : queue() {};

        virtual ~Scheduler();

        /**
         * Should be run every tick. It checks and runs* the tasks.
         * Routine tasks are run continiously
         * Normal tasks are run once then deleted
         * @param time
         */
        void tick(unsigned long time);

        /**
         * Add a task to the queue. Dont forget to delete the task if task didnt fit in the queue.
         * @param task
         * @return index of the task in the queue. -1 if queue is full.
         */
        int add_task(Task *task);

        /**
         * Removes the task from the queue. Made to be run internally. Use is_running instead
         * @param i
         */
        void remove_task(unsigned int i);
    };
}

#endif //INTER_TECH_SCHED_H
