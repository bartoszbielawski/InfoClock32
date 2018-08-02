#include <Arduino.h>
#include <webServer.h>
#include <SPIFFS.h>
#include <config_utils.h>
#include <utils.h>
#include <displayTask.h>
#include <lhcTask.h>
#include <task_utils.h>
#include <wifiConnectorTask.h>
#include <PathConstructor.hpp>

void owmTask(void*);
void lhcStatusTask(void*);
void serialCommandTask(void*);
void fixerIoTask(void*);

String getWeatherDescription();

TaskScheduler::Register serialCommandTaskR(new Task("SCT", &serialCommandTask));

TaskScheduler::Register owmTaskR(new Task("WFT", &owmTask, 8192, 5, Task::CONNECTED));
TaskScheduler::Register lhcStatusTaskR(new Task("LHC", &lhcStatusTask, 8192, 5, Task::CONNECTED));


void setup() {
    Serial.begin(115200);
    if (not SPIFFS.begin())
        SPIFFS.format();

    readConfigFromFS();

    int timeZoneOffset = getConfigValue("timezone", "0").toInt();
    configTime(timeZoneOffset, 0, "pool.ntp.org", "time.nist.gov", "ntp3.pl");

    TaskScheduler::addTask(&dt);
    TaskScheduler::addTask(&getWiFiConnectorTask());
    TaskScheduler::addTask(new Task("FIO", &fixerIoTask, 32768, 4, Task::CONNECTED));

    logPrintf("Found %zu tasks...", TaskScheduler::getTasks().size());

    for(auto* t: TaskScheduler::getTasks())
    {
        if (t->getFlags() & Task::CONNECTED)
            continue;       //skip "connected" tasks
            
        logPrintf("Starting task %s", t->getName());
        t->resume();
    }
}

void loop() 
{
    delay(10);
}