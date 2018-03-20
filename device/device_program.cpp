//
// Created by egordm on 19-3-2018.
//

#include "device_program.h"
#include "macros.h"
#include "config.h"

void DeviceProgram::init() {
    config_manager.setAPName(DEFAULT_AP_SSID);
    config_manager.begin(config);

    SHOUT(WiFi.localIP());
}

void DeviceProgram::tick(unsigned long time) {
    config_manager.loop();
    Scheduler::tick(time);
}