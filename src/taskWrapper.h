#ifndef TASK_WRAPPER_H
#define TASK_WRAPPER_H

#include <Arduino.h>

class Task
{
    public:
        enum Flags
        {
            NONE = 0,
            CONNECTED = 1
        };

        Task(String name, TaskFunction_t function, size_t stackSize = 8192, UBaseType_t priority = 5, uint32_t flags = NONE);
        virtual ~Task();

        void start();
        void restart();
        void suspend();
        void resume();


        const String& getName() const {return name;}
        uint32_t getFlags() const {return flags;}
    protected:
        TaskFunction_t function;
        String name;
        size_t stackSize;
        UBaseType_t priority;
        uint32_t flags;
        TaskHandle_t handle;
};

#endif //TASK_WRAPPER_H