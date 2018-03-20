//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_TASKS_H
#define B3ITASSIGNMENT2_TASKS_H


#include <PubSubClient.h>
#include "../lib/scheduler/scheduler.h"
#include "macros.h"

class MQTTRoutine : public internals::RoutineTask {
public:
    MQTTRoutine(unsigned long interval, PubSubClient &client) : RoutineTask(interval), client(client) {}

protected:
    PubSubClient &client;
};

class MQTTReconnectCycle : public MQTTRoutine {
public:
    MQTTReconnectCycle(PubSubClient &client) : MQTTRoutine(MQTT_CONNECT_POLL_INTERVAL, client) {}

private:
    bool execute() override;

    bool should_run(unsigned long time) override;
};


#endif //B3ITASSIGNMENT2_TASKS_H
