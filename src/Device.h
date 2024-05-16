#pragma once
#include "Instance.h"
#include "Queue.h"
#include "Structs.h"
#include <functional>
#include <string>
#include <set>

namespace vg
{
    /**
     *@brief Represents GPU device
     * Handles both physical and logical device
     */
    class Device
    {
    public:
        /**
         *@brief Device Type
         *
         */
        enum class Type
        {
            Other = 0,
            Integrated = 1,
            Dedicated = 2,
            Virtual = 3,
            Cpu = 4
        };


    public:
        /**
         *@brief Construct a new Device object
         *
         * @param queues Required queues
         * @param extensions Required extensions
         * @param surface Optional surface needed when device is used to render to a window
         * @param scoreFunction Function for scroing each device, function should return score or -1 if device is not an option
         */
        Device(
            const std::set<Queue::Type>& queues,
            const std::set<std::string>& extensions = {},
            SurfaceHandle surface = {},
            std::function<int(const std::set<Queue::Type>& supportedQueues, const std::set<std::string>& supportedExtensions, Type type, const Limits& limits)> scoreFunction = nullptr);
        /**
         *@brief Construct a new Device object
        *
        * @param queues Required queues
        * @param extensions Required extensions
        * @param scoreFunction Function for scroing each device, function should return score or -1 if device is not an option
        */
        Device(
            const std::set<Queue::Type>& queues,
            const std::set<std::string>& extensions = {},
            std::function<int(const std::set<Queue::Type>& supportedQueues, const std::set<std::string>& supportedExtensions, Type type, const Limits& limits)> scoreFunction = nullptr)
            :Device::Device(queues, extensions, {}, scoreFunction) {}

        Device();
        Device(Device&& other) noexcept;

        Device(const Device& other) = delete;
        ~Device();

        Device& operator=(Device&& other) noexcept;
        Device& operator=(const Device& other) = delete;

        operator const DeviceHandle& () const;
        operator const PhysicalDeviceHandle& () const;

        /**
         *@brief Waits until device is idle
         *
         */
        void WaitUntilIdle();

    public:
        DeviceHandle m_handle;
        PhysicalDeviceHandle m_physicalDevice;

    public:
        Queue graphicsQueue;
        Queue computeQueue;
        Queue presentQueue;
        Queue transferQueue;
    };

    extern Device currentDevice;
}