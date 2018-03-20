#ifndef DEFINES_H
#define DEFINES_H

#define DEBUG true

#define DEFAULT_AP_SSID "Planetron"

#define MQTT_CONNECT_POLL_INTERVAL 5000
#define SENSOR_POLL_INTERVAL 2000
#define WATER_PLANT_POLL_INTERVAL 200
#define WATER_PLANT_DURATION (3000/200)

#define WIRE_SCK D5
#define WIRE_SDA D3

#define SPB_SEL D0
#define SPB_ANALOG A0

#define MQTT_ROOT_TOPIC String("devices/assignment_device")

#define WATER_PLANT_TOPIC "devices/assignment_device/interaction/water_plant"
#define FORCE_SENSE_TOPIC "devices/assignment_device/interaction/force_sense"

#endif //DEFINES_H
