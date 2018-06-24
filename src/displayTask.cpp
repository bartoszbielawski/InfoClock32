#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include "displayTask.h"
#include "utils.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Time.h>
#include <WiFi.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <gfx_utils.h>

DisplayTask::DisplayTask()
{
    xTaskCreate(&DisplayTask::rtTask, "DisplayTask", 8192, this, 5, &handle);
    vTaskSuspend(handle);
}

DisplayTask::~DisplayTask()
{
    vTaskDelete(&handle);
}

void DisplayTask::addCyclicMessage(const MessageProvider& msg)
{
     messages.push_back(msg);
}


const static int cols = 25;

const String emptyLine("                                               ");

DisplayTask dt;

void displayTime(Adafruit_GFX& display)
{
    display.setTextSize(1);
    display.setFont();
    display.setCursor(0,0);
    display.printf("%s", getFormattedDateTime("%m-%d %H:%M"));
}

void DisplayTask::rtTask(void* that)
{
    logPrintf("Display task starting...");

    DisplayTask* o = static_cast<DisplayTask*>(that);

    Wire.begin(4,15);
    Adafruit_SSD1306 display(16);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

    Wire.setClock(800000);
    display.clearDisplay();
 
    display.setTextColor(WHITE);
  
    while(true)
    {   
        display.clearDisplay();
        displayTime(display);

        auto wifiMode = WiFi.getMode();

        const char* wifiState = "?";

        logPrintf("WiFiMode: %d", wifiMode);

        switch (wifiMode)
        {
            case  WIFI_MODE_STA:
                wifiState = WiFi.isConnected()? "OK": "X";
                break;
            case  WIFI_MODE_APSTA:
            case  WIFI_MODE_AP:
                wifiState = "AP";
                break;
            default:
                wifiState = "?";
        };

        display.setCursor(100, 0);
        display.println(wifiState);

        String& cm = o->currentMessage;

        if (cm.length() == 0)
        {
            if (o->priorityMessages.size())
            {
                logPrintf("Loading new priority message...");
                cm = o->priorityMessages.front();
                o->priorityMessages.pop();
            }
            else
            {
                logPrintf("Loading new regular message...");
                cm = o->messages[o->currentMessageIndex++](); 
                o->currentMessageIndex = o->currentMessageIndex % o->messages.size();
            }
            logPrintf("Message: %s", cm.c_str());
        }

        const static int canvasHeight = 20;

        auto len = cm.length() * 15;
        GFXcanvas1Read canvas(len, canvasHeight);
        canvas.setTextSize(1);
        canvas.setFont(&FreeSans9pt7b);
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
        canvas.getTextBounds((char*)cm.c_str(), 0, canvasHeight * 3/4, &x, &y, &w, &h);
        canvas.setCursor(0, canvasHeight * 3/4);
        canvas.print(cm);

        //logPrintf("x: %d y: %d w: %d h:%d", x, y, w, h);
        
        const static int step = 3;

        if (w > display.width())
        {
            
            for (int sx = 0; sx < w - display.width() + step; sx += step)
            {
                copyBitmap(canvas, sx, y, display, 0, 20, display.width(), h);
                display.display();
                delay(sx == 0 ? 500: 10);
            }     
        }
        else
        {
            int offset = (display.width() - w) / 2;
            copyBitmap(canvas, 0, y, display, offset, 20, display.width(), h);
            display.display();
            delay(500);
        }

        delay(500);
        cm = String();
    }
}
