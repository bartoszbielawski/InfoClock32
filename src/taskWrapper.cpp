#include <taskWrapper.h>


Task::Task(String name, TaskFunction_t function, size_t stackSize, UBaseType_t priority, uint32_t flags):
    function(function), name(name), stackSize(stackSize), priority(priority), flags(flags), handle(nullptr)
{

}

Task::~Task()
{
    if (handle)
        vTaskDelete(handle); 
    
    handle = nullptr;
}
    
void Task::start()
{
    if (handle)
        return;     //already started

    xTaskCreate(function, name.c_str(), stackSize, this, priority, &handle);
}

void Task::restart()
{
    if (handle) 
        vTaskDelete(handle);
    handle = nullptr;
    start();
}

void Task::suspend()
{
    if (handle)
        vTaskSuspend(handle);  
}

void Task::resume()
{
    if (handle)
        vTaskResume(handle);
}

    