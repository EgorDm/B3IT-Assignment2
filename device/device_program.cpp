//
// Created by egordm on 19-3-2018.
//

#include "device_program.h"
#include "macros.h"
#include "config.h"
#include "tasks.h"


void DeviceProgram::init() {
    config_manager.setAPName(DEFAULT_AP_SSID);
    setup_config(config_manager);

    client.setServer(config.mqtt_broker, static_cast<uint16_t>(config.mqtt_port));
    client.setCallback(std::bind(&DeviceProgram::data_recieved, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    add_task(new MQTTReconnectCycle(client));

    SHOUT(WiFi.localIP());
}

void DeviceProgram::tick(unsigned long time) {
    config_manager.loop();
    Scheduler::tick(time);
}

void DeviceProgram::connect_mqtt() {

}

void DeviceProgram::data_recieved(char *topic, byte *payload, unsigned int length) {

}
