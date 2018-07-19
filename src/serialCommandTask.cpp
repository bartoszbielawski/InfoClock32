#include <Arduino.h>
#include <utils.h>
#include <config_utils.h>

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
        logPrintf(F("CMD: %s"), command.c_str());

        if (command.startsWith("$"))
        {
            auto p = splitLine(command.substring(1));
            setConfigValue(p.first, p.second);
        }
    }
}
