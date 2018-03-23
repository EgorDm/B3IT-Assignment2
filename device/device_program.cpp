//
// Created by egordm on 19-3-2018.
//

#include "device_program.h"
#include "display_task.h"

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
    add_task(new ButtonToggleRoutine(client, config.automatic_mode, D3, AUTOMATIC_MODE_TOPIC));
    add_task(new AutomaticMaintenanceRoutine(plant_routine));
    add_task(new DisplayRoutine(plant_routine));

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
    } else if (strcmp(topic, AUTOMATIC_MODE_TOPIC) == 0) {
        config.automatic_mode = *payload == '1';
        SHOUT(String("Toggling automatic mode to ") + config.automatic_mode);
    }
}
