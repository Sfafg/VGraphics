#pragma once
#include "Handle.h"
#include "Device.h"
#include <vector>

namespace vg
{
    /**
     *@brief Used to synchronize CPU and GPU
     *
     */
    class Fence
    {
    public:
        /**
         *@brief Construct a new Fence object
         *
         * @param device Device
         * @param createSignalled If true then fence will start in signalled state
         */
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
    /**
     *@brief Await an array of fences
     *
     * @param device Device
     * @param fences Array of fences
     * @param awaitAll If true then it will return only when all fences are signalled
     * @param timeout Timeout in nanoseconds
     */
    void WaitForFences(const Device& device, const std::vector<FenceHandle>& fences, bool awaitAll = true, uint64_t timeout = UINT64_MAX);
    /**
     *@brief Reset all fences unsignalling them
     *
     * @param device Device
     * @param fences Array of Fences
     */
    void ResetFences(const Device& device, const std::vector<FenceHandle>& fences);

    /**
     *@brief Used to synchronize GPU with GPU processes
     *
     */
    class Semaphore
    {
    public:
        /**
         *@brief Construct a new Semaphore object
         *
         * @param device Device
         */
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