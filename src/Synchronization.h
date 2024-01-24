#pragma once
#include "Handle.h"
#include "Device.h"
#include <vector>

namespace vg
{
    class Fence
    {
    public:
        Fence(const Device& device, bool createSignalled = false);

        Fence();
        Fence(Fence&& other) noexcept;
        Fence(const Fence& other) = delete;
        ~Fence();

        Fence& operator=(Fence&& other) noexcept;
        Fence& operator=(const Fence& other) = delete;
        operator const FenceHandle& () const;

    private:
        FenceHandle m_handle;
        DeviceHandle m_device;
    };
    void WaitForFences(const Device& device, const std::vector<FenceHandle>& fences, bool awaitAll = true, uint64_t timeout = UINT64_MAX);
    void ResetFences(const Device& device, const std::vector<FenceHandle>& fences);

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