#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

const uint16_t D = 1;
const uint16_t S = 2;
const uint16_t SD = S | D;

void logPrintf(const char* fmt,...);
void logPrintf(const __FlashStringHelper* fmt, ...);
void logPrintf(uint16_t mode, const char* fmt, ...);
void logPrintf(uint16_t mode, const __FlashStringHelper* fmt, ...);

const char* getDateTime();
const char* getFormattedDateTime(const char* fmt);
const char* getUpTime();

#include <map>
#include <functional>

using StringLookup = std::function<const String&(const String&)>;
StringLookup generateMapLookup(const std::map<String, String>& m, bool enableConfig = true);

#endif //UTILS_H