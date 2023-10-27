#pragma once
#include "Instance.h"
#include "Queue.h"
#include "VulkanStructs.h"
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
        Queue graphicsQueue;
        Queue computeQueue;
        Queue presentQueue;
        Queue transferQueue;

    public:
        Device();
        Device(
            const std::set<Queue::Type>& queues,
            const std::set<std::string>& extensions = {},
            SurfaceHandle surface = {},
            std::function<int(const std::set<Queue::Type>& supportedQueues, const std::set<std::string>& supportedExtensions, Type type, const Limits& limits)> scoreFunction = nullptr);

        Device(const Device& other);
        Device(Device&& other) noexcept;
        Device& operator=(const Device& other);
        Device& operator=(Device&& other) noexcept;

        ~Device();

        operator DeviceHandle() const;
        operator PhysicalDeviceHandle() const;

    public:
        DeviceHandle m_handle;
        PhysicalDeviceHandle m_physicalDevice;
    };
}