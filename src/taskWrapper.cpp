#include <taskWrapper.h>


Task::Task(String name, TaskFunction_t function, size_t stackSize, UBaseType_t priority, uint32_t flags):
    function(function), stackSize(stackSize), priority(priority), flags(flags), handle(nullptr)
{
    //UBaseType_t previousPriority = uxTaskPriorityGet(NULL);
    xTaskCreate(function, name.c_str(), stackSize, this, priority, &handle);
    // vTaskSuspend(handle);
    // vTaskPrioritySet(handle, priority);
    // vTaskPrioritySet(NULL, previousPriority);
}

Task::~Task()
{
    vTaskDelete(handle); 
}
    

void Task::suspend()
{
    vTaskSuspend(handle);  
}

void Task::resume()
{
    vTaskResume(handle);
}

    