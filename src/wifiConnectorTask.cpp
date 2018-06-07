#include <wifiConnectorTask.h>
#include <ESPAsyncWiFiManager.h>

#include "webServer.h"

void WiFiEvent(WiFiEvent_t event, system_event_info_t info){
    switch(event)
    {
        case SYSTEM_EVENT_STA_START:
            Serial.println("Station Mode Started");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.println("Connected to " + String(WiFi.SSID()));
            Serial.printf("Got IP: %s\n", WiFi.localIP().toString().c_str());
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from station, attempting reconnection");
            WiFi.reconnect();
            break;
        default:
            Serial.printf("Something else... %d\n", (int)event);
            break;
    }
}

void configureWifi()
{
    delay(1000);
 
    WiFi.enableAP(false);
    WiFi.enableSTA(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    WiFi.onEvent(WiFiEvent);

    DNSServer dnsServer;
    AsyncWiFiManager wifiManager(&getWebServer(),&dnsServer);

    wifiManager.setBreakAfterConfig(true);
    wifiManager.setConnectTimeout(15);
    wifiManager.setConfigPortalTimeout(120);    //seconds
    wifiManager.setDebugOutput(false);
    //reset settings - for testing
    //wifiManager.resetSettings();

    wifiManager.autoConnect();
}