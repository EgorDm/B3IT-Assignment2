//
// Created by egordm on 19-3-2018.
//

#include "tasks.h"
#include "config.h"

bool MQTTReconnectCycle::execute() {
    SHOUT("Connecting to MQTT broker...");
    if(client.connect("", config.mqtt_username, config.mqtt_password)) {
        SHOUT("Succesfully connected to MQTT broker.");
        client.publish("outTopic", "hello world");
    } else {
        SHOUT("Failexd to connect to MQTT broker. State: " + String(client.state()));
    }
    return false;
}

bool MQTTReconnectCycle::should_run(unsigned long time) {
    return RoutineTask::should_run(time) && !client.connected();
}
