//
// Created by egordm on 19-3-2018.
//

#include "tasks.h"

bool MQTTReconnectCycle::execute() {
    SHOUT("Connecting to MQTT broker...");
    if (client.connect("", config.mqtt_username, config.mqtt_password, ALIVE_TOPIC, 2, 1, "0")) {
        SHOUT("Succesfully connected to MQTT broker.");
        client.subscribe(WATER_PLANT_TOPIC);
        client.subscribe(FORCE_SENSE_TOPIC);
        client.subscribe(AUTOMATIC_MODE_TOPIC);

        client.publish(ALIVE_TOPIC, "1", true);
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
    if (sensor_data.water_ticks == 0) sensor_data.last_watered = millis();

    if(sensor_data.water_ticks <= WATER_PLANT_DURATION/2) {
        servo.write(90);
    } else {
        servo.write(0);
    }

    ++sensor_data.water_ticks;

    SHOUT("Watering the plant!");

    return false;
}

bool WaterPlantRoutine::should_run(unsigned long time) {
    auto run = RoutineTask::should_run(time);
    if(!run) servo.write(0);
    return run && sensor_data.water_ticks < WATER_PLANT_DURATION;
}

bool AutomaticMaintenanceRoutine::execute() {
    if(sensor_data.soil_moisture < 10) plant_routine->start_watering();

    return false;
}

bool AutomaticMaintenanceRoutine::should_run(unsigned long time) {
    return RoutineTask::should_run(time) && config.automatic_mode;
}
