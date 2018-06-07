#include <Arduino.h>
#include <SPIFFS.h>

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> 
#include <DNSServer.h>

#include <Esp.h>

AsyncWebServer webServer(80);
DNSServer dnsServer;

AsyncWebServer& getWebServer()
{
    return webServer;
}

void configureWebServer()
{
    Serial.println("Configuring server...");
    webServer.onNotFound(
         [](AsyncWebServerRequest *request)
         {
             request->send(404, "plain/text", "Not found :(");
         });

    webServer.serveStatic("/files/", SPIFFS, "/");
    webServer.on("/", HTTP_GET, 
        [](AsyncWebServerRequest *request)
        {
            request->send(200, "text/plain", "I'm working!");
        }
    );
    webServer.begin();
    Serial.println("Done...");
}