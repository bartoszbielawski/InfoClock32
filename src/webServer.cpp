#include <Arduino.h>
#include <SPIFFS.h>

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> 
#include <DNSServer.h>

#include <Esp.h>
#include <config_utils.h>
#include <utils.h>

AsyncWebServer webServer(80);
DNSServer dnsServer;

AsyncWebServer& getWebServer()
{
    return webServer;
}


static void authenticate(AsyncWebServerRequest* request)
{
    if (not request->authenticate("user", getConfigValue("userPassword", "password").c_str()))
    {
        request->requestAuthentication();
    }
}

void handleRoot(AsyncWebServerRequest *request)
{
    request->send(SPIFFS, "/templates/index.html", String(), false, [](const String& key) 
    {
        return getConfigValue(key, "1.0.0");
    }
    );
}

void handleConfigEditor(AsyncWebServerRequest* request)
{
    authenticate(request);
   
    auto file = SPIFFS.open("/config.txt", "r");
    auto contents = file.readString();
    file.close();

    request->send(SPIFFS, "/templates/configEditor.html", "text/html", false, [&contents](const String& k) {return contents;});
}

void handleConfigSave(AsyncWebServerRequest* request)
{
    authenticate(request);
    
    if (!request->hasArg(F("content")))
        request->redirect("/");

    const String content = request->arg("content");

    auto file = SPIFFS.open("/config.txt", "w");
    file.print(content);
    file.close();

    request->send(200, "text/plain", "Rebooting...");

    delay(5000);
    ESP.reset();
}

void configureWebServer()
{
    logPrintf("WebServer config - start");
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
    logPrintf("WebServer config - done...");
}


