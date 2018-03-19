#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "macros.h"
#include "defines.h"

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", WEBPAGE_HTML);
}

void setup_router() {
  server.on("/", handleRoot);
}

void setup() {
  #if DEBUG
  Serial.begin(9600);
  #endif

  WiFi.mode(WIFI_AP);
  WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASSWORD);

  setup_router();

  server.begin();
}

void loop() {
  server.handleClient();
}