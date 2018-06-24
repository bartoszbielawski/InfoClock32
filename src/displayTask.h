#ifndef DISPLAY_TASK_H
#define DISPLAT_TASK_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include <queue>

using MessageProvider =  std::function<String()>;

class DisplayTask
{
    public:
        DisplayTask();
        ~DisplayTask();

        void run() {
            vTaskResume(handle);
            }

        void addCyclicMessage(const MessageProvider& msg);

        template <class S>
        void addMessage(S&& s)
        {
            priorityMessages.push(std::forward<S>(s));
        }

    private:
        static void rtTask(void*);
        TaskHandle_t handle;

        std::vector<MessageProvider> messages;
        std::queue<String> priorityMessages;
        String currentMessage;
        int currentMessageIndex = 0;
};

extern DisplayTask dt;

#endif //DISPLAY_TASK_H