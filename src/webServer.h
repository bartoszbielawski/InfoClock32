#ifndef WEBSERVER_H
#define WEBSERVER_H

void configureWebServer();

class AsyncWebServer;

AsyncWebServer& getWebServer();

#endif