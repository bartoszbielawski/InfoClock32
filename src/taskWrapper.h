#ifndef TASK_WRAPPER_H
#define TASK_WRAPPER_H

#include <Arduino.h>

class Task
{
    public:
        Task(String name, TaskFunction_t function, size_t stackSize = 2048, UBaseType_t priority = 5);
        virtual ~Task();

        void start();
        void restart();
        void suspend();
        void resume();

    protected:
        TaskFunction_t function;
        String name;
        size_t stackSize;
        UBaseType_t priority;
        TaskHandle_t handle;
};

#endif //TASK_WRAPPER_H