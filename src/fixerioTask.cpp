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
            sleep(180);
            continue;
        }

        MapCollector mc;

        {

            // auto httpStream = httpClient.getStream();    
            // while (httpStream.available())
            // {
            //     char c = httpStream.read();
            //     mc.parse(c);
            //     delay(1);
            // }

            
            auto httpString = httpClient.getString();
            httpString.trim();
            for (size_t i = 0; i < httpString.length(); ++i)
            {
                mc.parse(httpString[i]);              
            }
        }

        // logPrintf(F("FIO: Done parsing JSON..."));
        auto& values = mc.getValues();
        // for (const auto& p: values)
        // {
        //     logPrintf(F("%s => %s"), p.first.c_str(), p.second.c_str());
        // }

        auto success = values["/root/success"] == "true" ? true: false;
        if (not success)
        {
            logPrintf(F("Problem getting values: %s"), values["/root/error/code"].c_str());
            sleep(3600);
            continue;
        }

        String convRatio = values[std::string("/root/rates/") + toCurrency].c_str(); 
        
        String newMsg = fromCurrency;
        newMsg += "->";
        newMsg += toCurrency;
        newMsg += ": ";
        newMsg += convRatio;

        fixerioMessage = newMsg;

        logPrintf(F("FIO: %s"), newMsg.c_str());

        sleep(3600);
    }
}