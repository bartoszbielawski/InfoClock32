#include <Arduino.h>
#include <webServer.h>
#include <SPIFFS.h>
#include <config_utils.h>
#include <utils.h>
#include <displayTask.h>
#include <lhcTask.h>
#include <task_utils.h>

void configureWifi();
void wifiTask(void*);

String getWeatherDescription();

void setup() {
    Serial.begin(115200);
    if (not SPIFFS.begin())
        SPIFFS.format();

    dt.addCyclicMessage(getLHCState);
    dt.addCyclicMessage(getWeatherDescription);
    
    readConfigFromFS();

    xTaskCreate(wifiTask, "WiFiTask", 4096, NULL, 2, NULL);

    int timeZoneOffset = getConfigValue("timezone", "0").toInt();
    configTime(timeZoneOffset, 0, "pool.ntp.org", "time.nist.gov", "ntp3.pl");

    
    for(auto* t: TaskScheduler::getTasks())
    {
        t->start();
        delay(100);
        t->resume();
    }
}

void loop() 
{
    logPrintf("I'm alive...");
    delay(60000);
}