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

#endif //MACROS_H