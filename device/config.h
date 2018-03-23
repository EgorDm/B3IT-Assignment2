//
// Created by egordm on 19-3-2018.
//

#ifndef B3ITASSIGNMENT2_CONFIG_H
#define B3ITASSIGNMENT2_CONFIG_H

#include <ConfigManager.h>

struct Config {
    char mqtt_broker[40];
    short mqtt_port;
    char mqtt_client_id[20];
    char mqtt_username[20];
    char mqtt_password[20];
    char rx_topic[60];
    char tx_topic[60];
    bool enable_pw;
    bool enable_debug;
    bool automatic_mode;
};

extern Config config;

struct SensorData {
    float temparature;
    float pressure;
    float humidity;
    int soil_moisture;
    int light;
    unsigned long last_watered;
    int water_ticks;
};

extern SensorData sensor_data;


extern void setup_config(ConfigManager &config_manager);

#endif //B3ITASSIGNMENT2_CONFIG_H
