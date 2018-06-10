

#include <HTTPClient.h>
#include "utils.h"
#include "lhcTask.h"
#include <config_utils.h>
#include <webServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <rtos_utils.h>

static const char pageUrl[] PROGMEM = "http://alicedcs.web.cern.ch/AliceDCS/monitoring/screenshots/rss.xml";

static Semaphore lhcStatusSemaphore;

struct LHCState
{
    LHCState() {clear();}
    
    String combinedPage1Comment;
    String energy;
    String lastUpdate;

    void clear()
    {
        SemaphoreLocker<Semaphore> locker(lhcStatusSemaphore);
        combinedPage1Comment = F("No Page1 comment...");
        energy = F("Unknown");
        lastUpdate = getDateTime();
    }

    LHCState getCopy()
    {
        SemaphoreLocker<Semaphore> locker(lhcStatusSemaphore);
        return *this;
    }
};

LHCState lhcState;

// String getPage1Comment()
// {
//     SemaphoreLocker locker(lhcStatusSemaphore);
//     return lhcState.combinedPage1Comment;
//     
// }

// String getBeamEnergy()
// {
//     SemaphoreLocker locker(lhcStatusSemaphore);
//     return lhcState.energy;
//     if (not beamEnergy.length())
//         return "Energy: ???";
//     String s("Energy: ");
//     s += beamEnergy;
//     return s;
// }

// String readFullLine(Stream& s)
// {
//     String result;
//     while (not result.endsWith("</title>"))
//     {
//         result += s.readStringUntil('\n');
//     }

//     return result;
// }


void lhcHandleRequest(AsyncWebServerRequest *request)
{
   auto localCopy = lhcState.getCopy();

    const std::map<String,String> m = {
        {"p1comment", localCopy.combinedPage1Comment},
        {"energy", localCopy.energy},
        {"updated", localCopy.lastUpdate}
    };

    request->send(SPIFFS, "/templates/lhcStatus.html", "text/html", false, generateMapLookup(m, true));
}

void lhcStatusTask(void*)
{
    bool enabled = getConfigValue("lhc.enabled", "0").toInt();
    if (!enabled)
    {
        logPrintf("LHC Task disabled, exiting...");
        vTaskDelete(NULL);
        return;
    }

    logPrintf("LHC Status Task Starting!");

    auto webServer = getWebServer();
    webServer.on("/lhcStatus", &lhcHandleRequest);

    while (true)
    {
        HTTPClient httpClient;
	    httpClient.begin(pageUrl);

        logPrintf("Reading LHC Status...");

        int httpCode = httpClient.GET();
        if (httpCode != HTTP_CODE_OK)
        {
            logPrintf("Couldn't get URL: %d\n", httpCode);
            lhcState.clear();
            delay(60000);
            continue;
        }

	    auto httpStream = httpClient.getStream();

        LHCState newLHCState;
        String beamMode;

        while (httpStream.available())
        {
            httpStream.findUntil("<title>","</rss>");
            auto title = httpStream.readStringUntil(':');
            if (title == F("LhcPage1"))
            {
                String& newPage1Comment = newLHCState.combinedPage1Comment;
                newPage1Comment = httpStream.readStringUntil('<');
                newPage1Comment.trim();
                newPage1Comment.replace(F("\n\n"), F("\n"));
            }

            if (title == F("BeamEnergy"))
            {
                newLHCState.energy = httpStream.readStringUntil('<');
                newLHCState.energy.trim();
            }

            if (title == F("LhcBeamMode"))
            {
                beamMode = httpStream.readStringUntil('<');
                beamMode.trim();
            }
        }

        {
            newLHCState.combinedPage1Comment = beamMode + ": " +  newLHCState.combinedPage1Comment;
            SemaphoreLocker<Semaphore> locker(lhcStatusSemaphore);
            lhcState = newLHCState;
        }

        delay(60000);
    }
}