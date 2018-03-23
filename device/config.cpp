//
// Created by egordm on 20-3-2018.
//


#include <ConfigManager.h>
#include "config.h"

Config config;

SensorData sensor_data;

void setup_config(ConfigManager &config_manager) {
    config_manager.addParameter("broker", config.mqtt_broker, 40);
    config_manager.addParameter("port", &config.mqtt_port);
    config_manager.addParameter("cid", config.mqtt_client_id, 20);
    config_manager.addParameter("username", config.mqtt_username, 20);
    config_manager.addParameter("password", config.mqtt_password, 20);
    config_manager.addParameter("rx-topic", config.rx_topic, 60);
    config_manager.addParameter("tx-topic", config.tx_topic, 60);
    config_manager.addParameter("pw-enable", &config.enable_pw);
    config_manager.addParameter("debug_enable", &config.enable_debug);
    config_manager.begin(config);
}
