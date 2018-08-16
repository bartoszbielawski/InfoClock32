#include <task_utils.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <rtos_utils.h>
#include <utils.h>

void localSensorTaskFunction(void*)
{
    vTaskSuspend(NULL);

    Adafruit_BME280 bme280Sensor;
    if (!bme280Sensor.begin())
    {
        logPrintf(F("LST: couldn't initialize the sensor!"));
        while(1) sleep(60);
    }

    while (true)
    {
        {
            SemaphoreLocker<Semaphore> sl(i2cSemaphore);        
            float temperature = bme280Sensor.readTemperature();
            float pressure = bme280Sensor.readPressure();
            float humidity = bme280Sensor.readHumidity();
            logPrintf(F("T = %.2f, p = %f"), temperature, pressure);
        }
        sleep(10);
    }
}