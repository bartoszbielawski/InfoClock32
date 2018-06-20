#include <wifiConnectorTask.h>
#include <ESPAsyncWiFiManager.h>

#include "webServer.h"
#include <utils.h>
#include <config_utils.h>

#include <map>

const std::map<WiFiEvent_t, String> descr =
{
    {SYSTEM_EVENT_WIFI_READY,               "ESP32 WiFi ready"},
    {SYSTEM_EVENT_SCAN_DONE,                "ESP32 finish scanning AP"},
    {SYSTEM_EVENT_STA_START,                "ESP32 station start"},
    {SYSTEM_EVENT_STA_STOP,                 "ESP32 station stop"},
    {SYSTEM_EVENT_STA_CONNECTED,            "ESP32 station connected to AP"},
    {SYSTEM_EVENT_STA_DISCONNECTED,         "ESP32 station disconnected from AP"},
    {SYSTEM_EVENT_STA_AUTHMODE_CHANGE,      "the auth mode of AP connected by ESP32 station changed"},
    {SYSTEM_EVENT_STA_GOT_IP,               "ESP32 station got IP from connected AP"},
    {SYSTEM_EVENT_STA_LOST_IP,              "ESP32 station lost IP and the IP is reset to 0"},
    {SYSTEM_EVENT_STA_WPS_ER_SUCCESS,       "ESP32 station wps succeeds in enrollee mode"},
    {SYSTEM_EVENT_STA_WPS_ER_FAILED,        "ESP32 station wps fails in enrollee mode"},
    {SYSTEM_EVENT_STA_WPS_ER_TIMEOUT,       "ESP32 station wps timeout in enrollee mode"},
    {SYSTEM_EVENT_STA_WPS_ER_PIN,           "ESP32 station wps pin code in enrollee mode"},
    {SYSTEM_EVENT_AP_START,                 "ESP32 soft-AP start"},
    {SYSTEM_EVENT_AP_STOP,                  "ESP32 soft-AP stop"},
    {SYSTEM_EVENT_AP_STACONNECTED,          "a station connected to ESP32 soft-AP"},
    {SYSTEM_EVENT_AP_STADISCONNECTED,       "a station disconnected from ESP32 soft-AP"},
    {SYSTEM_EVENT_AP_PROBEREQRECVED,        "Receive probe request packet in soft-AP interface"},
    {SYSTEM_EVENT_GOT_IP6,                  "ESP32 station or ap or ethernet interface v6IP addr is preferred"}
};

void WiFiEvent(WiFiEvent_t event, system_event_info_t info)
{
    logPrintf("WIFI: %s", descr.at(event).c_str());
}



void configureWifi()
{
    delay(1000);

    WiFi.onEvent(WiFiEvent);

    DNSServer dnsServer;
    AsyncWiFiManager wifiManager(&getWebServer(),&dnsServer);

    wifiManager.setBreakAfterConfig(true);
    wifiManager.setConnectTimeout(30);
    wifiManager.setConfigPortalTimeout(120) ;    //seconds
    wifiManager.setDebugOutput(false);

    logPrintf(SD, F("MAC Address: %s"), WiFi.macAddress().c_str());
    //reset settings - for testing
    //wifiManager.resetSettings();

    if (not wifiManager.autoConnect())
    {
        logPrintf(SD, F("Couldn't connect to WiFi, resetting..."));
        sleep(5);
        ESP.restart();
    }
}