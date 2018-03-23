#ifndef MACROS_H
#define MACROS_H

#include "defines.h"

#if DEBUG
#define SHOUT(something) (Serial.println((something)))
#else
#define SHOUT(something) ()
#endif

#define TOPIC(topic) (MQTT_ROOT_TOPIC + (topic)).c_str()

#define SENSOR_TXT(name, value, unit) (String((name)) + (value) + String((unit)))

#endif //MACROS_H