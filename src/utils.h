#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

void logPrintf(const char* fmt,...);
void logPrintf(const __FlashStringHelper* fmt, ...);
const char* getDateTime();
const char* getUpTime();

#include <map>
#include <functional>

using StringLookup = std::function<const String&(const String&)>;
StringLookup generateMapLookup(const std::map<String, String>& m, bool enableConfig = true);

#endif //UTILS_H