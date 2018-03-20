//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_DEVICE_PROGRAM_H
#define B3ITASSIGNMENT2_DEVICE_PROGRAM_H

#include <ESP8266WebServer.h>
#include <ConfigManager.h>
#include <PubSubClient.h>
#include "../lib/scheduler/scheduler.h"
#include "config.h"

class DeviceProgram : public internals::Scheduler {
private:
    ESP8266WebServer server;
    WiFiClient wifi_client;
    PubSubClient client;
    ConfigManager config_manager;

public:
    DeviceProgram() : server(80), client(wifi_client) {}

    void init();

    void tick(unsigned long time) override;

    void connect_mqtt();

    void data_recieved(char* topic, byte* payload, unsigned int length);
};


#endif //B3ITASSIGNMENT2_DEVICE_PROGRAM_H
