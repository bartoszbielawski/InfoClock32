#include "config_utils.h"
#include "SPIFFS.h"
#include <utils.h>
#include <utility>

#include <map>

using namespace std;

static std::map<String, String> configValues;

const std::map<String, String> getConfigValues() {return configValues;}

pair<String, String> splitLine(String&& line)
{
    pair<String, String> result;

    line.trim();     

    if (line.length() == 0)
        return result;

    if (line[0] == '#')
        return result;

    auto pos = line.indexOf('=');   //find the first character

    if (pos == -1)
    {
        result.first = line;
        return result;
    }

    result.first = line.substring(0, pos);
    line.remove(0, pos+1);          //remove the equal sign as well
    result.second = line;
    return result;
}

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
        auto p = splitLine( file.readStringUntil('\n'));
        if (not p.second.length())
            continue;
            
        logPrintf("Config: %s = %s", p.first.c_str(), p.second.c_str());
        configValues[p.first] = p.second;
    }
}   

const String& getConfigValue(const String& name, const String& dflt)
{
    auto i = configValues.find(name);
    if (i == configValues.end()) 
        return dflt;
    
    return i->second;
}

void setConfigValue(const String& key, const String& value)
{
    configValues[key] = value;
}