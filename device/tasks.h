//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_TASKS_H
#define B3ITASSIGNMENT2_TASKS_H


#include <PubSubClient.h>
#include <Adafruit_BME280.h>
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
    explicit MQTTReconnectCycle(PubSubClient &client) : MQTTRoutine(MQTT_CONNECT_POLL_INTERVAL, client) {}

private:
    bool execute() override;

    bool should_run(unsigned long time) override;
};

class BME20SensorRoutine : public MQTTRoutine {
public:
    explicit BME20SensorRoutine(PubSubClient &client) : MQTTRoutine(BME20_POLL_INTERVAL, client) {
        if (!bme.begin()) SHOUT("BME280 sensor is not found!");
        else started = true;
    }

private:
    Adafruit_BME280 bme;
    bool started = false;

    bool execute() override;
};

#endif //B3ITASSIGNMENT2_TASKS_H
