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