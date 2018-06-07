#include <Arduino.h>
#include <webServer.h>
#include <SPIFFS.h>

void configureWifi();

void setup() {
    Serial.begin(115200);
    SPIFFS.begin();
    configureWifi();
    configureWebServer();
    Serial.println("Init done!");
    
}

int i = 0;

void loop() 
{
    Serial.printf("%d\n", i++);
    delayMicroseconds(1000000);
}