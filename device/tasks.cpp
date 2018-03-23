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
        client.subscribe(AUTOMATIC_MODE_TOPIC);
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

    sensor_data.temparature = bme.readTemperature();
    sensor_data.pressure = bme.readPressure() / 100.0F;
    sensor_data.humidity = bme.readHumidity();
    digitalWrite(SPB_SEL, HIGH);
    sensor_data.soil_moisture = analogRead(SPB_ANALOG);
    digitalWrite(SPB_SEL, LOW);
    sensor_data.light = analogRead(SPB_ANALOG);

    client.publish(TOPIC("/sensor/temperature"), String(sensor_data.temparature).c_str(), true);
    client.publish(TOPIC("/sensor/pressure"), String(sensor_data.pressure).c_str(), true);
    client.publish(TOPIC("/sensor/humidity"), String(sensor_data.humidity).c_str(), true);
    client.publish(TOPIC("/sensor/soil_moisture"), String(sensor_data.soil_moisture).c_str(), true);
    client.publish(TOPIC("/sensor/light"), String(sensor_data.light).c_str(), true);

    return false;
}

bool WaterPlantRoutine::execute() {
    if (ticks == 0) last_watered = millis();
    ++ticks;

    SHOUT("Watering the plant!");

    return false;
}

bool WaterPlantRoutine::should_run(unsigned long time) {
    return RoutineTask::should_run(time) && ticks < WATER_PLANT_DURATION;
}

bool AutomaticMaintenanceRoutine::execute() {
    return false;
}

bool AutomaticMaintenanceRoutine::should_run(unsigned long time) {
    return RoutineTask::should_run(time) && config.automatic_mode;
}
