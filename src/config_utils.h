#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include <Arduino.h>

void readConfigFromFS();
const String& getConfigValue(const String& name, const String& dflt);

#endif //CONFIG_UTILS_H