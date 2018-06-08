

#include <HTTPClient.h>
#include "utils.h"
#include "lhcTask.h"

static const char pageUrl[] PROGMEM = "http://alicedcs.web.cern.ch/AliceDCS/monitoring/screenshots/rss.xml";

String page1Comment;
String beamEnergy;
String beamMode;

String getPage1Comment()
{
    if (beamMode.length() && (not page1Comment.length()))
        return beamMode;
    if (not (beamMode.length() && page1Comment.length()))
        return "No Page1 Comment";
    String m(beamMode);
    m += ": ";
    m += page1Comment;
    return m;
}

String getBeamEnergy()
{
    if (not beamEnergy.length())
        return "Energy: ???";
    String s("Energy: ");
    s += beamEnergy;
    return s;
}

String readFullLine(Stream& s)
{
    String result;
    while (not result.endsWith("</title>"))
    {
        result += s.readStringUntil('\n');
    }

    return result;
}


void lhcStatusTask(void*)
{
    logPrintf("LHC Status Task Starting!");
    while (true)
    {
        HTTPClient httpClient;
	    httpClient.begin(pageUrl);

        logPrintf("Reading LHC Status...");

        int httpCode = httpClient.GET();
        if (httpCode != HTTP_CODE_OK)
        {
            logPrintf("Couldn't get URL: %d\n", httpCode);
            delay(5000);
            continue;
        }

	    auto httpStream = httpClient.getStream();

        while (httpStream.available())
        {
            bool found = httpStream.findUntil("<title>","</rss>");

            auto title = httpStream.readStringUntil(':');
            if (title == F("LhcPage1"))
            {
                String newPage1Comment = httpStream.readStringUntil('<');
                newPage1Comment.trim();
                newPage1Comment.replace(F("\n\n"), F("\n"));
                if (newPage1Comment != page1Comment)
                {
                    page1Comment = newPage1Comment;
                }
            }

            if (title == F("BeamEnergy"))
            {
                beamEnergy = httpStream.readStringUntil('<');
                beamEnergy.trim();
            }

            if (title == F("LhcBeamMode"))
            {
                beamMode = httpStream.readStringUntil('<');
                beamMode.trim();
            }
        }

        logPrintf("%s - %s", getBeamEnergy().c_str(), getPage1Comment().c_str());
        delay(60000);
    }
}