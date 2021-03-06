
/*
 * WeatherGetter.cpp
 *
 *  Created on: 04.01.2017
 *      Author: Bartosz Bielawski
 */

#include "config.h"
#include "utils.h"
#include "config_utils.h"
#include <MapCollector.hpp>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <pgmspace.h>
#include <SPIFFS.h>
#include <webServer.h>
#include <task_utils.h>
#include <displayTask.h>

/*
 * 2660646 - Geneva
 * 2659667 - Meyrin
 * 2974936 - Sergy
 * 6424612 - Ornex
 */

using namespace std;

const static char urlTemplate[] PROGMEM =
		"http://api.openweathermap.org/data/2.5/weather?id=%d&APPID=%s&units=metric";
const static char urlForecastTemplate[] PROGMEM =
		"http://api.openweathermap.org/data/2.5/forecast?id=%d&APPID=%s&units=metric&cnt=2";

void owmHandleStatus(AsyncWebServerRequest *request);

String getHTMLWeatherDescription();
String getTextWeatherDescription();

/* forecast path
 /root/list/n/main/temp			--temperature
 /root/list/n/weather/0/main	--description
 /root/list/n/weather/0/description --
 */

static const vector<String> prefixes{
	"main/temp",
	"name",
	"list/1/main/temp",
	"list/1/weather/0/description"
};

static bool jsonPathFilter(const string& key, const string& /*value*/)
{
	String s(key.c_str());

	for (auto& prefix: prefixes)
	{
		//"/root/" => first 6 chars
		if (s.startsWith(prefix, 6))
			return true;
	}

	return false;
}

int getHttpResponse(HTTPClient& httpClient, MapCollector& mc, const char* url)
{
	httpClient.begin(url);
	int httpCode = httpClient.GET();

	if (httpCode != 200)
		return httpCode;

	mc.reset();

	String json = httpClient.getString();
	for (size_t  i = 0; i < json.length(); ++i)
	{
		mc.parse(json[i]);
	}

	httpClient.end();

	return httpCode;
}

struct Weather
{
    Weather(uint32_t locationId):
        locationId(locationId),
        temperature(0),
        temperatureForecast(0) {}

    uint32_t locationId;
    float  temperature;
    float  temperatureForecast;
    String description;
    String location;
};

std::vector<Weather> weathers;

void owmTask(void*)
{
    vTaskSuspend(NULL);
    weathers.clear();

    String owmAPIKey = getConfigValue("owm.apikey", String());
    logPrintf(F("OWM: Starting..."));
    {
        String owmIds = getConfigValue("owm.ids", String());   
        if ((owmIds.length() == 0) or (owmAPIKey.length() == 0))
        {
            logPrintf("OWM not configured!");
            sleep(5);
            vTaskDelete(NULL);
            return;
        }
        
        //generate list of "weathers" to be checked
        uint32_t from = 0;
        int32_t to;

        do
        {
            auto commaIndex = owmIds.indexOf(",", from);
            to = commaIndex == -1 ? owmIds.length(): commaIndex;

            String s = owmIds.substring(from, to);
            logPrintf(F("OWM: Found ID: %s"), s.c_str());
            from = to+1;
            weathers.emplace_back(s.toInt());
        }
        while (from < owmIds.length());
        logPrintf(F("OWM: Found %d IDs"), weathers.size());
    }

    getDisplayTask().addCyclicMessage(&getTextWeatherDescription);
    getWebServer().on("/owmStatus", &owmHandleStatus);

    while (true)
    {
        HTTPClient httpClient;
        httpClient.setReuse(true);
        for (auto& w: weathers)
        {
            logPrintf(F("OWM: Reading weather for id = %d"), w.locationId);

            //locals needed for the rest of the code
            MapCollector mc(jsonPathFilter);
           
            char localBuffer[256];
            int code = 0;

            //get weather
            snprintf_P(localBuffer, sizeof(localBuffer), urlTemplate, w.locationId, owmAPIKey.c_str());
            logPrintf(F("OWM: URL: %s"), localBuffer);
            code = getHttpResponse(httpClient, mc, localBuffer);
            if (code != 200)
            {
                logPrintf("OWM: HTTP Code %d", code);
                sleep(5);
                continue;
            }

            auto& results = mc.getValues();
            w.temperature = atof(results["/root/main/temp"].c_str());
            w.location = results["/root/name"].c_str();

            //get forecast
            snprintf_P(localBuffer, sizeof(localBuffer), urlForecastTemplate, w.locationId, owmAPIKey.c_str());
            logPrintf(F("OWM: URL: %s"), localBuffer);
            code = getHttpResponse(httpClient, mc, localBuffer);
            if (code != 200)
            {
                logPrintf("OWM: HTTP Code %d", code);
                sleep(5);
                continue;
            }

            w.temperatureForecast = atof(results["/root/list/1/main/temp"].c_str());
            w.description = results["/root/list/1/weather/0/description"].c_str();
            
            logPrintf(F("OWM: Done reading for id = %d"), w.locationId);

            httpClient.end();
        
            sleep(10);
        }
        sleep(600);
    }
}

/*
arguments will be:
    %s - location,
    %s - temperature now,
    %s - temperature in 3-6 hours,
    %s - weather description
*/

const static char htmlFormat[] = "<tr><td id='h'>%s</td><td id='i'>%s&deg;C (%s&deg;C, %s)</td></tr>";
const static char plainTextFormat[] = "%s - %s*C (%s*C, %s)"; 

String getFormattedWeatherDescription(const char* fmt, const char* separator)
{
    char buffer[128];
    String result;
    result.reserve(512);

    for (const auto& w: weathers)
    {
        if (w.location.length() == 0)
            continue;

        snprintf(buffer, sizeof(buffer), fmt,
            w.location.c_str(),
            String(w.temperature,1).c_str(),
            String(w.temperatureForecast, 1).c_str(),
            w.description.c_str());

        result += buffer;
        result += separator;
    }

    if (result.length() == 0)
        return F("No weather forecast available...");

    int separatorLen = strlen(separator);
    result.remove(result.length()-separatorLen, separatorLen);

    return result;
}

String getHTMLWeatherDescription()
{
    return getFormattedWeatherDescription(htmlFormat, "");
}

String getTextWeatherDescription()
{
   return getFormattedWeatherDescription(plainTextFormat, " -- ");
}

void owmHandleStatus(AsyncWebServerRequest *request)
{
    String x = getHTMLWeatherDescription();
    request->send(SPIFFS, "/templates/owmStatus.html", "text/html", false,  [&](const String& k) {return x;});
}



