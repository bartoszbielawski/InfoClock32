#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include <Arduino.h>
#include <utility>
#include <map>

using namespace std;

const std::map<String, String> getConfigValues();

void readConfigFromFS();
std::pair<String, String> splitLine(String&& line);

const String& getConfigValue(const String& name, const String& dflt);
void setConfigValue(const String& key, const String& value);

#endif //CONFIG_UTILS_H