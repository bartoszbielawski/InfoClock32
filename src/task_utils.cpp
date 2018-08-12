#include <task_utils.h>
#include <functional>
#include <algorithm>

std::vector<Task*> TaskScheduler::tasks;

void TaskScheduler::callOnTasks(Task::Flags f, std::function<void(Task*)> fun)
{
    for (auto* t: getTasks())
    {
        if (!(t->getFlags() & f))
            continue;

        fun(t);
    }    
}

Task* TaskScheduler::getTaskByName(const String& name)
{
    for (auto* t: getTasks())
    {
        if (strcmp(t->getName(), name.c_str()) == 0)
            return t;
    }

    return nullptr;
}