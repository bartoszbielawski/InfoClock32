#include <utils.h>
#include <time.h>
#include <config_utils.h>
#include <rtos_utils.h>
#include <displayTask.h>
#include <ctime>

static Semaphore loggingSemaphore;


void logPrint(const char* string, uint16_t mode)
{
    if (mode == 0) mode = S;
    if (mode & S)
    {
        Serial.printf("%s: ",getDateTime());
        Serial.println(string);
    }
    if (mode & D)
    {
        getDisplayTask().addMessage(string);
    }
}

void logPrintf(uint16_t mode, const char* fmt,...)
{
    SemaphoreLocker<Semaphore> locker(loggingSemaphore);
    char localBuffer[128];
    va_list args;
    va_start (args, fmt);

    vsnprintf(localBuffer, sizeof(localBuffer), fmt, args);

    va_end (args);

    logPrint(localBuffer, mode);
}

void logPrintf(uint16_t mode, const __FlashStringHelper* fmt, ...)
{
    SemaphoreLocker<Semaphore> locker(loggingSemaphore);
    char localBuffer[128];
    va_list args;
    va_start (args, fmt);

    vsnprintf(localBuffer, sizeof(localBuffer), (const char*)fmt, args);

    va_end (args);

    logPrint(localBuffer, mode);
}

void logPrintf(const char* fmt,...)
{
    SemaphoreLocker<Semaphore> locker(loggingSemaphore);
    char localBuffer[128];
    va_list args;
    va_start (args, fmt);

    vsnprintf(localBuffer, sizeof(localBuffer), fmt, args);

    va_end (args);

    logPrint(localBuffer, SERIAL);
}


void logPrintf(const __FlashStringHelper* fmt, ...)
{
    SemaphoreLocker<Semaphore> locker(loggingSemaphore);
    char localBuffer[128];
    va_list args;
    va_start (args, fmt);

    vsnprintf(localBuffer, sizeof(localBuffer), (const char*)fmt, args);

    va_end (args);

    logPrint(localBuffer, SERIAL);
}


static char dateTimeString[22] = "1970-01-01 00:00:01";

static time_t startTime = 0;

const char* getDateTime()
{ 
    static time_t lastPrinted = 0;
    time_t now = time(NULL);
    if (lastPrinted == now)
        return dateTimeString;
    
    if ((startTime == 0) && now > 1000000000)       //after year 2000
        startTime = now;            //save the boot time    

    lastPrinted = now;

	auto lt = localtime(&now);
    snprintf(dateTimeString, sizeof(dateTimeString), "%d-%02d-%02d %02d:%02d:%02d",
            lt->tm_year + 1900,
            lt->tm_mon + 1,
            lt->tm_mday,
			lt->tm_hour,
			lt->tm_min,
            lt->tm_sec);
    
    return dateTimeString;
}

static char b[128];

const char* getFormattedDateTime(const char* fmt)
{
    time_t now = time(NULL);
	auto lt = localtime(&now);
    strftime(b, 128, fmt, lt);
    return b;
}

static char upTimeString[32] = "???";

const char* getUpTime()
{
    if (not startTime)
        return upTimeString;

    time_t now = time(NULL);
    int upTime = now - startTime;

    //days
    div_t d = div(upTime, 24 * 60 * 60);
    int days = d.quot;

    //hours
    d = div(d.rem, 60 * 60);
    int hours = d.quot;

    //minutes
    d = div(d.rem, 60);
    int minutes = d.quot;
    int seconds = d.rem;

    snprintf(upTimeString, sizeof(upTimeString), "%dd%02dh%02dm%02ds", days, hours, minutes, seconds);

    return upTimeString;
}

const String questionMarks = "???";

StringLookup generateMapLookup(const std::map<String, String>& m, bool enableConfig)
{
    StringLookup lookup = [&](const String& key) -> const String&
    {
        auto i = m.find(key);
        if (i != m.end())
            return i->second;

        if (enableConfig)
            return getConfigValue(key, questionMarks);

        return questionMarks;
    };

    return lookup;
}