#ifndef RTOS_UTILS
#define RTOS_UTILS

#include <freertos/semphr.h>

template <class T>
class SemaphoreLocker
{
    public:
        SemaphoreLocker(T& handle): handle(handle)
        {
            handle.lock();
        }

        SemaphoreLocker(const SemaphoreLocker& other) = delete;
        SemaphoreLocker(SemaphoreLocker&& other) = delete;
        SemaphoreLocker& operator=(const SemaphoreLocker& other) = delete;

        ~SemaphoreLocker() 
        {
            handle.unlock();
        }
    
    private:
        T& handle;
};

struct Semaphore
{
    Semaphore() 
    {
        handle = xSemaphoreCreateMutex();
    }
    ~Semaphore() {unlock();}
    Semaphore(const Semaphore& other) = delete;

    void lock() {if (handle) xSemaphoreTake(handle, portMAX_DELAY);}
    void unlock() {if (handle) xSemaphoreGive(handle);}
    void reset() {handle = NULL;}

    SemaphoreHandle_t handle;
};

#endif //RTOS_UTILS