//
// Created by egordm on 19-3-2018.
//

#include "tasks.h"
#include "config.h"
#include "../cvision/macros.h"

bool MQTTReconnectCycle::execute() {
    SHOUT("Connecting to MQTT broker...");
    if (client.connect("", config.mqtt_username, config.mqtt_password)) {
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

bool BME20SensorRoutine::execute() {
    if (!started) return true;

    client.publish(TOPIC("/sensor/temperature"), String(bme.readTemperature()).c_str(), false);
    client.publish(TOPIC("/sensor/pressure"), String(bme.readPressure() / 100.0F).c_str(), false);
    client.publish(TOPIC("/sensor/humidity"), String(bme.readHumidity()).c_str(), false);

    return false;
}
