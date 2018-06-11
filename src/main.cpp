#include <Arduino.h>
#include <webServer.h>
#include <SPIFFS.h>
#include <config_utils.h>
#include <utils.h>

void configureWifi();
void lhcStatusTask(void*);
void owmTask(void*);

void setup() {
    Serial.begin(115200);
    SPIFFS.begin();
    
    readConfigFromFS();
    configureWifi();

    int timeZoneOffset = getConfigValue("timezone", "0").toInt();
    configTime(timeZoneOffset, 0, "pool.ntp.org", "time.nist.gov", "ntp3.pl");
    configureWebServer();
    
    logPrintf("Task creation...");
    xTaskCreate(lhcStatusTask, "LHCTask", 4096, NULL, 5, NULL);
    xTaskCreate(owmTask, "OWM", 4096, NULL, 5, NULL);
}

void loop() 
{
    logPrintf("I'm alive...");
    delay(60000);
}