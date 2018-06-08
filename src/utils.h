#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

void logPrintf(const char* fmt,...);
void logPrintf(const __FlashStringHelper* fmt, ...);
const char* getDateTime();
const char* getUpTime();

#endif //UTILS_H