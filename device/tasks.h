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

class SensorRoutine : public MQTTRoutine {
public:
    explicit SensorRoutine(PubSubClient &client) : MQTTRoutine(SENSOR_POLL_INTERVAL, client) {
        if (!bme.begin()) SHOUT("BME280 sensor is not found!");
        else started = true;
    }

    bool execute() override;

private:
    Adafruit_BME280 bme;
    bool started = false;
};

class WaterPlantRoutine : public internals::RoutineTask {
public:
    WaterPlantRoutine() : RoutineTask(WATER_PLANT_POLL_INTERVAL), ticks(WATER_PLANT_DURATION) {};
private:
    unsigned int ticks;

public:
    bool execute() override;

    bool should_run(unsigned long time) override;

    void start_watering() {if(ticks >= WATER_PLANT_DURATION) ticks = 0;}
};

#endif //B3ITASSIGNMENT2_TASKS_H
