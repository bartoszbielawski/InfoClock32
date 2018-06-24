#include <Arduino.h>
#include <webServer.h>
#include <SPIFFS.h>
#include <config_utils.h>
#include <utils.h>
#include <displayTask.h>
#include <lhcTask.h>

void configureWifi();
void lhcStatusTask(void*);
void owmTask(void*);
void i2cScannerTask(void*);
void wifiTask(void*);

String getWeatherDescription();

void setup() {
    Serial.begin(115200);
    if (not SPIFFS.begin())
        SPIFFS.format();

    dt.addCyclicMessage(getLHCState);
    dt.addCyclicMessage(getWeatherDescription);
    dt.run();
    
    readConfigFromFS();

    xTaskCreate(wifiTask, "WiFiTask", 2048, NULL, 2, NULL);

    int timeZoneOffset = getConfigValue("timezone", "0").toInt();
    configTime(timeZoneOffset, 0, "pool.ntp.org", "time.nist.gov", "ntp3.pl");
    //configureWebServer();
    
    xTaskCreate(lhcStatusTask, "LHCTask", 4096, NULL, 5, NULL);
    xTaskCreate(owmTask, "OWM", 4096, NULL, 5, NULL);
}

void loop() 
{
    logPrintf("I'm alive...");
    delay(60000);
}