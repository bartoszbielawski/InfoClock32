#include "config_utils.h"
#include "SPIFFS.h"
#include <utils.h>

#include <map>

using namespace std;

static std::map<String, String> configValues;

void readConfigFromFS()
{
    logPrintf(F("Reading configuration values from the flash..."));
    //the FS has to be initialized already...
    auto file = SPIFFS.open("/config.txt", "r");
    if (!file)
        return;
    
    configValues.clear();

    while (file.available())
    {
        String line = file.readStringUntil('\n');
        line.trim();        
        //skip empty lines
        if (line.length() == 0)
            continue;

        //skip comments
        if (line[0] == '#')
            continue;

        auto pos = line.indexOf('=');   //find the first character

        if (pos == -1)
            continue;                   //skip the line, it doesn't contain = sign

        String key = line.substring(0, pos);
        line.remove(0, pos+1);          //remove the equal sign as well
        String value = line;


        logPrintf("Config: %s = %s", key.c_str(), value.c_str());
        configValues[key] = value;
    }
}   

const String& getConfigValue(const String& name, const String& dflt)
{
    auto i = configValues.find(name);
    if (i == configValues.end()) 
        return dflt;
    
    return i->second;
}