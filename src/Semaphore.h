#pragma once
#include "Handle.h"

namespace vg
{
    class Semaphore
    {
    public:
        Semaphore();
        Semaphore(const Semaphore& other);
        Semaphore(Semaphore&& other) noexcept;
        Semaphore& operator=(const Semaphore& other);
        Semaphore& operator=(Semaphore&& other) noexcept;
        ~Semaphore();


    private:
        SemaphoreHandle m_handle;
        DeviceHandle m_device;
    };
}