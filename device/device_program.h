//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_DEVICE_PROGRAM_H
#define B3ITASSIGNMENT2_DEVICE_PROGRAM_H

#include <ESP8266WebServer.h>
#include <ConfigManager.h>
#include "../lib/scheduler/scheduler.h"

class DeviceProgram : public internals::Scheduler {
private:
    ESP8266WebServer server;
    ConfigManager config_manager;

public:
    DeviceProgram() : server(80), config_manager() {}

    void init();

    void tick(unsigned long time) override;
};


#endif //B3ITASSIGNMENT2_DEVICE_PROGRAM_H
