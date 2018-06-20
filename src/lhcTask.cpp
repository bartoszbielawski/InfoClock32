

#include <HTTPClient.h>
#include "utils.h"
#include "lhcTask.h"
#include <config_utils.h>
#include <webServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <rtos_utils.h>

static const char pageUrl[] PROGMEM = "http://alicedcs.web.cern.ch/AliceDCS/monitoring/screenshots/rss.xml";

static Semaphore semaphore;

struct LHCState
{
    LHCState() {clear();}
    
    String page1Comment;
    String beamMode;
    String energy;
    String lastUpdate;

    void clear()
    {
        SemaphoreLocker<Semaphore> locker(semaphore);
        page1Comment = F("No Page1 comment...");
        beamMode = F("???");
        energy = F("???");
        lastUpdate = getDateTime();
    }

    String getCombinedComment()
    {
        SemaphoreLocker<Semaphore> locker(semaphore);
        return beamMode + ": " + page1Comment;
    }

    String getDescription()
    {
        SemaphoreLocker<Semaphore> locker(semaphore);
        String result = beamMode + ": " + page1Comment;
        result += " -- ";
        result += energy;
        return result;
    } 
};

static LHCState lhcState;

String getLHCState() 
{
    return lhcState.getDescription();
}


void lhcHandleRequest(AsyncWebServerRequest *request)
{
   auto localCopy = lhcState;

    const std::map<String,String> m = {
        {"p1comment", localCopy.beamMode + ": " + localCopy.page1Comment},
        {"energy", localCopy.energy},
        {"updated", localCopy.lastUpdate}
    };

    request->send(SPIFFS, "/templates/lhcStatus.html", "text/html", false, generateMapLookup(m, true));
}

void lhcStatusTask(void*)
{
    logPrintf("LHC: Status Task Starting!");

    bool enabled = getConfigValue("lhc.enabled", "0").toInt();
    if (!enabled)
    {
        logPrintf("LHC: Task disabled, exiting...");
        vTaskDelete(NULL);
        return;
    }

    auto webServer = getWebServer();
    webServer.on("/lhcStatus", &lhcHandleRequest);

    while (true)
    {
        HTTPClient httpClient;
	    httpClient.begin(pageUrl);

        logPrintf(F("LHC: Reading status..."));

        int httpCode = httpClient.GET();
        if (httpCode != HTTP_CODE_OK)
        {
            logPrintf("LHC: Couldn't get URL: %d", httpCode);
            lhcState.clear();
            delay(60000);
            continue;
        }


        //this map will collect values from the XML file for us
        LHCState newLHCStatus;
        std::map<String, String&> m = 
                {{"LhcPage1", newLHCStatus.page1Comment},
                 {"BeamEnergy", newLHCStatus.energy},
                 {"LhcBeamMode", newLHCStatus.beamMode}};

        auto httpStream = httpClient.getStream();
        while (httpStream.available())
        {
            httpStream.findUntil("<title>","</rss>");
            auto title = httpStream.readStringUntil(':');
            auto content = httpStream.readStringUntil('\n');
            content.replace("</title>","");
            content.trim();
            
            //check if we want to save this elements and if so store it in the map
            auto f = m.find(title);
            if (f != m.end()) (f->second) = content;
        }

        {
            SemaphoreLocker<Semaphore> locker(semaphore);
            lhcState = newLHCStatus;
        }

        logPrintf("LHC: %s", lhcState.getDescription().c_str());

        delay(60000);
    }
}