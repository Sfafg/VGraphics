#pragma once
#include "Instance.h"
#include "Queue.h"
#include "Structs.h"
#include <functional>
#include <string>
#include <set>

namespace vg
{
    class Device
    {
    public:
        enum class Type
        {
            Other = 0,
            Integrated = 1,
            Dedicated = 2,
            Virtual = 3,
            Cpu = 4
        };


    public:
        Device(
            const std::set<Queue::Type>& queues,
            const std::set<std::string>& extensions = {},
            SurfaceHandle surface = {},
            std::function<int(const std::set<Queue::Type>& supportedQueues, const std::set<std::string>& supportedExtensions, Type type, const Limits& limits)> scoreFunction = nullptr);
        Device(
            const std::set<Queue::Type>& queues,
            const std::set<std::string>& extensions = {},
            std::function<int(const std::set<Queue::Type>& supportedQueues, const std::set<std::string>& supportedExtensions, Type type, const Limits& limits)> scoreFunction = nullptr)
            :Device::Device(queues, extensions, {}, scoreFunction) {}

        Device();
        Device(Device&& other) noexcept;
        Device(const Device& other) = delete;
        ~Device();

        Device& operator=(const Device& other) = delete;
        Device& operator=(Device&& other) noexcept;

        operator const DeviceHandle& () const;
        operator const PhysicalDeviceHandle& () const;
        operator DeviceHandle& ();
        operator PhysicalDeviceHandle& ();

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
}