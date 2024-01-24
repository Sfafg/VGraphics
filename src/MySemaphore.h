#pragma once
#include "Handle.h"
#include "Device.h"

namespace vg
{
    class Semaphore
    {
    public:
        Semaphore(const Device& device);

        Semaphore();
        Semaphore(Semaphore&& other) noexcept;
        Semaphore(const Semaphore& other) = delete;
        ~Semaphore();

        Semaphore& operator=(Semaphore&& other) noexcept;
        Semaphore& operator=(const Semaphore& other) = delete;
        operator const SemaphoreHandle& () const;

    private:
        SemaphoreHandle m_handle;
        DeviceHandle m_device;
    };
}