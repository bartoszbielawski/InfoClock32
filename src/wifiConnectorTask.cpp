#include <wifiConnectorTask.h>
#include <ESPAsyncWiFiManager.h>

#include "webServer.h"
#include <utils.h>

void WiFiEvent(WiFiEvent_t event, system_event_info_t info){
    switch(event)
    {
        case SYSTEM_EVENT_STA_START:
            logPrintf(F("Station Mode Started"));
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            logPrintf(F("Connected to %s with %s"), 
                WiFi.SSID().c_str(),
                WiFi.localIP().toString().c_str());
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            logPrintf(F("Disconnected from station, attempting reconnection"));
            //WiFi.reconnect();
            break;
        default:
            logPrintf("Other WiFi event (%d)", (int)event);
            break;
    }
}

void configureWifi()
{
    delay(1000);
 
    logPrintf(F("Running WiFi manager..."));

    WiFi.enableAP(false);
    WiFi.enableSTA(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    WiFi.onEvent(WiFiEvent);

    DNSServer dnsServer;
    AsyncWiFiManager wifiManager(&getWebServer(),&dnsServer);

    wifiManager.setBreakAfterConfig(true);
    wifiManager.setConnectTimeout(120);
    wifiManager.setConfigPortalTimeout(120) ;    //seconds
    wifiManager.setDebugOutput(false);
    //reset settings - for testing
    //wifiManager.resetSettings();

    if (not wifiManager.autoConnect())
    {
        logPrintf(F("Couldn't connect to WiFi, resetting..."));
        sleep(5);
        ESP.restart();
    }
}