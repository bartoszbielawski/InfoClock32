#include <Arduino.h>
#include <webServer.h>
#include <SPIFFS.h>
#include <config_utils.h>
#include <utils.h>

void configureWifi();
void lhcStatusTask(void*);


void setup() {
    Serial.begin(115200);
    SPIFFS.begin();
    logPrintf(F("Reading configuration values from the flash..."));
    readConfigFromFS();
    logPrintf(F("Running WiFi manager..."));
    configureWifi();
    int timeZoneOffset = getConfigValue("timezone", "0").toInt();
    configTime(timeZoneOffset, 0, "pool.ntp.org", "time.nist.gov", "ntp3.pl");

    logPrintf(F("Starting webserver..."));
    configureWebServer();
    logPrintf(F("Init done!"));

    xTaskCreate(lhcStatusTask, "LHCTask", 2048, NULL, 5, NULL);
}

void loop() 
{
    logPrintf("I'm alive...");
    delay(60000);
}