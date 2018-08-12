#include <Arduino.h>
#include <time.h>
#include "displayTask.h"
#include "utils.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Time.h>
#include <WiFi.h>
#include <Wire.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <afbuffer.h>

DisplayTask::DisplayTask():    
    Task("DT", &DisplayTask::rtTask, 16384, 6)
{
    addCyclicMessage([](){return getFormattedDateTime("%H:%M");});
}

void DisplayTask::addCyclicMessage(const MessageProvider& msg)
{
     messages.push_back(msg);
}


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
    vTaskSuspend(NULL);

    logPrintf("Display task starting...");

    DisplayTask* o = static_cast<DisplayTask*>(that);

    Wire.begin(4,15);
    Adafruit_SSD1306 display(16);

    Wire.setClock(400000);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    display.setTextColor(WHITE);  
    while(true)
    {   
        display.clearDisplay();
        displayTime(display);

        const String ipAddress = WiFi.localIP().toString().c_str();
        String wifiState = "OK";

        if (ipAddress == "0.0.0.0")
            wifiState = "X";
        if (ipAddress == "192.168.4.1")
            wifiState = "AP";

        display.setCursor(100, 0);
        display.print(wifiState);

        String cm;

        if (o->priorityMessages.size())
        {
            cm = o->priorityMessages.front();
            o->priorityMessages.pop();
            logPrintf(F("DPL PM: %s"), cm.c_str());
        }
        else
        {
            cm = o->messages[o->currentMessageIndex++](); 
            o->currentMessageIndex = o->currentMessageIndex % o->messages.size();
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
    }
}
