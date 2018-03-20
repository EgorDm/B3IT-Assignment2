#ifndef DEFINES_H
#define DEFINES_H

#define DEBUG true

#define DEFAULT_AP_SSID "Planetron"

#define MQTT_CONNECT_POLL_INTERVAL 5000
#define BME20_POLL_INTERVAL 2000

#define WIRE_SCK D5
#define WIRE_SDA D3

#define SPB_SEL D0
#define SPB_ANALOG A0

#define MQTT_ROOT_TOPIC String("devices/assignment_device")

#endif //DEFINES_H
