#ifndef TASK_UTILS_H
#define TASK_UTILS_H

#include <taskWrapper.h>
#include <vector>


class TaskScheduler
{
    private:
        static std::vector<Task*> tasks;

    public:
        static const std::vector<Task*>& getTasks() {return TaskScheduler::tasks;}

        static void addTask(Task* t) {TaskScheduler::tasks.push_back(t);}

        struct Register{Register(Task* t) {TaskScheduler::addTask(t);}};
};


#endif // TASK_UTILS_H