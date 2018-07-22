#include <Arduino.h>
#include <config_utils.h>
#include <utils.h>
#include <HTTPClient.h>
#include <AJSP.hpp>
#include <MapCollector.hpp>
#include <string.h>
#include <displayTask.h>

using namespace std;

static const char fixerIOPageUrl[] PROGMEM = "http://data.fixer.io/api/latest?access_key=%s&base=%s&symbols=%s";


static String fixerioMessage(F("No currency data yet..."));

String getFixerIOMessage()
{
    return fixerioMessage;
}

void fixerIoTask(void*)
{
    vTaskSuspend(NULL);

    dt.addCyclicMessage(&getFixerIOMessage);

    sleep(10);

    if (getConfigValue("fio.key", String()) .length() == 0)
    {
        logPrintf(F("FIO: No valud API Key found, terminating..."));
        vTaskDelete(NULL);
    }

    char url[128];
    char fromCurrency[5];
    char toCurrency[5];

    {
            String to = getConfigValue(F("fio.to"), F("CHF"));
            String from = getConfigValue(F("fio.from"), F("EUR"));

            strncpy(toCurrency, to.c_str(), sizeof(toCurrency));
            strncpy(fromCurrency, from.c_str(), sizeof(fromCurrency));
    }

    snprintf_P(url, sizeof(url), fixerIOPageUrl, 
                                    getConfigValue(F("fio.key"), String()).c_str(),
                                    fromCurrency,
                                    toCurrency);
    
    logPrintf(F("FIO: URL:%s"), url);

    while (true)
    {
        HTTPClient httpClient;
    	logPrintf(F("FIO: Reading conversion rate..."));

	    httpClient.begin(url);

        int httpCode = httpClient.GET();
        if (httpCode != 200)
        {
            logPrintf(F("FIO: HTTP code: %d"), httpCode);
            sleep(3600);
            continue;
        }

        MapCollector mc;

        {
            auto httpString = httpClient.getString();
            for (size_t i = 0; i < httpString.length(); ++i)
            {
                mc.parse(httpString[i]);
            }
        }

        auto& values = mc.getValues();

        auto success = values["/root/success"] == "true" ? true: false;
        if (not success)
        {
            logPrintf(F("Problem getting values: %s"), values["/root/error/code"].c_str());
            sleep(3600);
            continue;
        }

        auto convRatio = values[std::string("/root/rates/") + toCurrency]; 
        
        String newMsg = fromCurrency;
        newMsg += "->";
        newMsg += toCurrency;
        newMsg += ": ";
        newMsg += convRatio.c_str();

        fixerioMessage = newMsg;

        logPrintf(F("%s"), newMsg.c_str());

        sleep(3600);
    }
}