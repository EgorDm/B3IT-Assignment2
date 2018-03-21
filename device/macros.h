#ifndef MACROS_H
#define MACROS_H

#include "defines.h"

#if DEBUG
#define SHOUT(something) (Serial.println((something)))
#else
#define SHOUT(something) ()
#endif

#define REDIRECT(url) \
    server.sendHeader("Location", (url), true); server.send(302, "text/plain", "");

#define TOPIC(topic) (MQTT_ROOT_TOPIC + (topic)).c_str()

#endif //MACROS_H