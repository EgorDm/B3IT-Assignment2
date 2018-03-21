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
        client.subscribe(WATER_PLANT_TOPIC);
        client.subscribe(FORCE_SENSE_TOPIC);
    } else {
        SHOUT("Failexd to connect to MQTT broker. State: " + String(client.state()));
    }
    return false;
}

bool MQTTReconnectCycle::should_run(unsigned long time) {
    return RoutineTask::should_run(time) && !client.connected();
}

bool SensorRoutine::execute() {
    if (!started) return true;

    client.publish(TOPIC("/sensor/temperature"), String(bme.readTemperature()).c_str(), true);
    client.publish(TOPIC("/sensor/pressure"), String(bme.readPressure() / 100.0F).c_str(), true);
    client.publish(TOPIC("/sensor/humidity"), String(bme.readHumidity()).c_str(), true);

    digitalWrite(SPB_SEL, HIGH);
    client.publish(TOPIC("/sensor/soil_moisture"), String(analogRead(SPB_ANALOG)).c_str(), true);

    digitalWrite(SPB_SEL, LOW);
    client.publish(TOPIC("/sensor/light"), String(analogRead(SPB_ANALOG)).c_str(), true);

    return false;
}

bool WaterPlantRoutine::execute() {
    ++ticks;

    SHOUT("Watering the plant!");

    return false;
}

bool WaterPlantRoutine::should_run(unsigned long time) {
    return RoutineTask::should_run(time) && ticks < WATER_PLANT_DURATION;
}
