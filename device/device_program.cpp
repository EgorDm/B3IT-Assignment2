//
// Created by egordm on 19-3-2018.
//

#include "device_program.h"
#include "macros.h"
#include "config.h"
#include "tasks.h"

/*
void callback(char* topic, byte* payload, unsigned int length) {
    SHOUT(String("Recieved topic: ") + topic);
    SHOUT(strcmp(topic, WATER_PLANT_TOPIC));
}
*/


void DeviceProgram::init() {
    Wire.begin(WIRE_SDA, WIRE_SCK);

    config_manager.setAPName(DEFAULT_AP_SSID);
    setup_config(config_manager);

    client.setServer(config.mqtt_broker, static_cast<uint16_t>(config.mqtt_port));
    client.setCallback(std::bind(&DeviceProgram::data_recieved, this, std::placeholders::_1, std::placeholders::_2,
                                 std::placeholders::_3));

    add_task(new MQTTReconnectCycle(client));
    sensor_routine = new SensorRoutine(client);
    plant_routine = new WaterPlantRoutine();
    add_task(sensor_routine);
    add_task(plant_routine);

    SHOUT(WiFi.localIP());
}

void DeviceProgram::tick(unsigned long time) {
    config_manager.loop();
    client.loop();
    Scheduler::tick(time);
}


void DeviceProgram::data_recieved(char *topic, byte *payload, unsigned int length) {
    SHOUT(String("Recieved topic: ") + topic);
    if (strcmp(topic, WATER_PLANT_TOPIC) == 0) {
        SHOUT("Starting watering plants.");
        plant_routine->start_watering();
    } else if (strcmp(topic, FORCE_SENSE_TOPIC) == 0) {
        SHOUT("Forcing the sensors to read.");
        sensor_routine->execute();
    }
}
