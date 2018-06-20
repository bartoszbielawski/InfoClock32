#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include "displayTask.h"
#include "utils.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Time.h>
#include <WiFi.h>

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

    DisplayTask* o = static_cast<DisplayTask*>(that);

    Wire.begin(4,15);
    Adafruit_SSD1306 display(16);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
    display.clearDisplay();

    //display.setFont(&Roboto_Mono_Medium_8);  
    display.setTextColor(WHITE);
    display.setTextSize(1);
  
    while(true)
    {   
        display.clearDisplay();
        display.setTextSize(1);
        
        display.setCursor(0,0);
        display.printf("%s", getFormattedDateTime("%m-%d %H:%M"));

        const char* wifiState = WiFi.status() == WL_CONNECTED? "OK": "X";
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

        // display.drawLine(0, 10, 128, 10, WHITE);
        // display.setCursor(0, 12);
        // display.println(cm);
        display.display();

        cm = String();

        // cm = cm.substring(lineEndIndex);
        // cm.trim();
    
        vTaskDelay(2000);
    }
}
