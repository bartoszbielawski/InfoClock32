#include <taskWrapper.h>


Task::Task(String name, TaskFunction_t function, size_t stackSize, UBaseType_t priority, uint32_t flags):
    function(function), stackSize(stackSize), priority(priority), flags(flags), handle(nullptr)
{
    _start(name);
}

Task::~Task()
{
    _kill();
}

void Task::_kill()
{
    vTaskDelete(handle);
    handle = nullptr;
}

void Task::_start(const String& name)
{
    xTaskCreate(function, name.c_str(), stackSize, this, priority, &handle);
}
        

void Task::restart()
{
    String name = getName();
    _kill();
    _start(name);
}

void Task::suspend()
{
    vTaskSuspend(handle);  
}

void Task::resume()
{
    vTaskResume(handle);
}

    