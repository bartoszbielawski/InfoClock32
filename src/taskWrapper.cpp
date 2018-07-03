#include <taskWrapper.h>


Task::Task(String name, TaskFunction_t function, size_t stackSize, UBaseType_t priority):
    function(function), name(name), stackSize(stackSize), priority(priority), handle(nullptr)
{

}

Task::~Task()
{

}
    
void Task::start()
{
    if (handle)
        return;     //already started

    xTaskCreate(function, name.c_str(), stackSize, this, priority, &handle);
}

void Task::restart()
{
    vTaskDelete(handle);
    handle = nullptr;
    start();
}

void Task::suspend()
{
    vTaskSuspend(handle);
}

void Task::resume()
{
    vTaskResume(handle);
}

    