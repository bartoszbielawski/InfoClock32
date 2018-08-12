#include <Arduino.h>
#include <utils.h>
#include <config_utils.h>
#include <task_utils.h>
#include <algorithm>
#include <wifiConnectorTask.h>

void serialCommandTask(void*)
{
    vTaskSuspend(NULL);
    while (true)
    {
        String command;

        bool commandComplete = false;
        do
        {
            while (Serial.available())
            {
                auto b = Serial.read();
                if (b == '\n')
                {
                    commandComplete = true;
                    break;
                }
                command += (char)b;
            }
            delay(100);
        } while (!commandComplete);

        command.trim();
        logPrintf(F("CMD: %s"), command.c_str());

        if (command.startsWith("$"))
        {
            auto p = splitLine(command.substring(1));
            setConfigValue(p.first, p.second);
            continue;
        }

        if (command == "vars")
        {
            for (const auto& p: getConfigValues())
            {
                logPrintf(F("%s = %s"), p.first.c_str(), p.second.c_str());
            }
            continue;
        }

        if (command == "reboot")
        {
            logPrintf(F("CMD: Rebooting device..."));
            ESP.restart();
            continue;   //not really needed
        }

        if (command.startsWith("restart="))
        {
            auto p = splitLine(command.substring(1));
            if (!p.second.length())
                continue;

            auto* t = TaskScheduler::getTaskByName(p.second);

            if (not t)
            {
                logPrintf(F("CMD: Task %s not found!"), p.second.c_str());
                continue;
            }

            t->restart();
            t->resume();
            continue;
        }

        if (command == "getfreeheap")
        {
            logPrintf(F("CMD: Free heap %d"), ESP.getFreeHeap());
            continue;
        }

        logPrintf(F("CMD: unknown command: %s"), command.c_str());
    }
}
