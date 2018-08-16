#include <Arduino.h>
#include "utils.h"
#include <Wire.h>
#include <rtos_utils.h>

void i2cScannerTaskFunction(void*)
{
    vTaskSuspend(NULL);
    
    logPrintf("I2C Scanner Task starting...");

    while(true)
    {
        for (int i = 0; i < 127; i++)
        {
            {
                SemaphoreLocker<Semaphore> sl(i2cSemaphore);
                Wire.beginTransmission(i);
                auto error = Wire.endTransmission();
                if (error == 0)
                {
                    logPrintf("Found I2C device at address 0x%02X", i);
                }
            }
            delay(100);
        }
        logPrintf("Done scanning...");
        delay(10000);
    }
}