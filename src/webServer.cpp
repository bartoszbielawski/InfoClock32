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

void handleRoot(AsyncWebServerRequest *request)
{
    request->send(SPIFFS, "/templates/index.html", String(), false);
}

void handleConfigEditor(AsyncWebServerRequest* request)
{
    if (not request->authenticate("admin", "admin"))
    {
        request->requestAuthentication();
    }
   
    auto file = SPIFFS.open("/config.txt", "r");
    auto contents = file.readString();

    Serial.println(contents);

    file.close();

    request->send(SPIFFS, "/templates/configEditor.html", "text/html", false, [&contents](const String& k) {return contents;});
}

void handleConfigSave(AsyncWebServerRequest* request)
{
    if (not request->authenticate("admin", "admin"))
    {
        request->requestAuthentication();
    }

    if (!request->hasArg(F("content")))
        request->redirect("/");

    const String content = request->arg("content");

    auto file = SPIFFS.open("/config.txt", "w");
    file.print(content);
    file.close();

    Serial.println(content);

    request->redirect("/");
}

void configureWebServer()
{
    Serial.println("Configuring server...");
    webServer.onNotFound(
         [](AsyncWebServerRequest *request)
         {
             request->send(404);
         });

    webServer.serveStatic("/static_files/", SPIFFS, "/static_files/");
    webServer.on("/", handleRoot);
    webServer.on("/configEditor", handleConfigEditor);
    webServer.on("/configSave", handleConfigSave);
    webServer.begin();
    Serial.println("Done...");
}

