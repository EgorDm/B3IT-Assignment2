#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "macros.h"
#include "defines.h"
#include "device_program.h"

DeviceProgram program;


void setup() {
#if DEBUG
    Serial.begin(9600);
#endif
    program.init();
}

void loop() {
   program.tick(millis());
}