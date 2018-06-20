#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include "displayTask.h"
#include "utils.h"

#include "SSD1306Wire.h"
#include <Adafruit_SSD1306.h>


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

void DisplayTask::rtTask(void* that)
{
    logPrintf("Display task starting...");

    pinMode(16, OUTPUT);
    digitalWrite(16, 0);
    delay(100); 
    digitalWrite(16, 1); 
    delay(100);

    DisplayTask* o = static_cast<DisplayTask*>(that);
    
    SSD1306Wire display(0x3C, 4, 15, GEOMETRY_128_64);
    display.init();
    display.flipScreenVertically();
   
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.displayOn();

    while(true)
    {   
        display.clear();
        display.drawString(0, 0, getDateTime());

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

        // int lineEndIndex = cols;
        // String fullLine = cm.substring(0, cols);

        // if ((fullLine.length() == cols) && (cm.length() != cols))
        // {
        //     //find last whitespace
        //     int lastWhitespace = fullLine.lastIndexOf(' ');
        //     if (lastWhitespace < 0) lastWhitespace = cols;

        //     int newLineIndex =   fullLine.indexOf('\n');
        //     if (newLineIndex < 0)   newLineIndex   = cols;
            
        //     lineEndIndex = std::min(lastWhitespace, newLineIndex);
        // }

        // String line = cm.substring(0, lineEndIndex);
        // line.trim();

        // auto tillEndOfLine = cols - line.length();
        // if (tillEndOfLine)
        // {
        //     line += emptyLine.substring(0,tillEndOfLine);
        // }

        //display.drawString(0, 16, line);
        display.drawStringMaxWidth(0, 16, 120, cm);
        display.display();

        cm = String();

        // cm = cm.substring(lineEndIndex);
        // cm.trim();
    
        vTaskDelay(2000);
    }
}
