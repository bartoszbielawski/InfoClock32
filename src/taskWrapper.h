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

        void suspend();
        void resume();

        TaskHandle_t& getTaskHandle() {return handle;}
        operator TaskHandle_t() const {return handle;}

        const char* getName() const {return pcTaskGetTaskName(handle);}
        uint32_t getFlags() const {return flags;}
    protected:
        void _start();
        TaskFunction_t function;
        size_t stackSize;
        UBaseType_t priority;
        uint32_t flags;
        TaskHandle_t handle;
};

#endif //TASK_WRAPPER_H