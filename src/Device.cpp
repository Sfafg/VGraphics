#include <vulkan/vulkan.hpp>
#include <vector>
#include "Device.h"

namespace vg
{
    int DefaultScoreFunction(
        const std::set<Queue::Type>& queues,
        const std::set<std::string>& extensions,
        const std::set<Queue::Type>& supportedQueues,
        const std::set<std::string>& supportedExtensions,
        Device::Type type,
        const Limits& limits)
    {
        for (const auto& queue : queues)
            if (!supportedQueues.contains(queue)) return -1;

        for (const auto& extension : extensions)
            if (!supportedExtensions.contains(extension)) return -1;

        return 0;
    }

    Device::Device(
        const std::set<Queue::Type>& queues,
        const std::set<std::string>& extensions,
        SurfaceHandle surface,
        std::function<int(const std::set<Queue::Type>& supportedQueues, const std::set<std::string>& supportedExtensions, Type type, const Limits& limits)> scoreFunction
    )
    {
        assert(queues.size() > 0);
        assert(queues.contains(Queue::Type::Present) ^ (surface == nullptr));

        // Pick physical device.
        int highestScore = -1;
        for (const auto& physicalDevice : ((InstanceHandle) vg::instance).enumeratePhysicalDevices())
        {
            std::set<std::string> supportedExtensions;
            for (const auto& extension : physicalDevice.enumerateDeviceExtensionProperties())
            {
                supportedExtensions.insert(extension.extensionName);
            }
            if (surface != nullptr && extensions.contains("VK_KHR_swapchain") && supportedExtensions.contains("VK_KHR_win32_surface"))
            {
                unsigned int formatCount, presentModeCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
                if (formatCount == 0 || presentModeCount == 0) continue;
            }

            std::set<Queue::Type> supportedQueues;
            auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            for (unsigned int i = 0; i < queueFamilies.size(); i++)
            {
                if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)supportedQueues.insert(Queue::Type::Graphics);
                if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) supportedQueues.insert(Queue::Type::Compute);
                if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer)supportedQueues.insert(Queue::Type::Transfer);
                if (surface != nullptr && physicalDevice.getSurfaceSupportKHR(i, surface)) supportedQueues.insert(Queue::Type::Present);

                if ((int) supportedQueues.size() == 3 + (surface != nullptr)) break;
            }

            Type type = (Type) physicalDevice.getProperties().deviceType;
            auto limits = physicalDevice.getProperties().limits;

            int score;
            if (scoreFunction != nullptr)
                score = scoreFunction(supportedQueues, supportedExtensions, type, *(Limits*) &limits);

            else
                score = DefaultScoreFunction(queues, extensions, supportedQueues, supportedExtensions, type, *(Limits*) &limits);

            if (score > highestScore)
            {
                m_physicalDevice = physicalDevice;
                highestScore = score;
            }
        }
        if (highestScore == -1)
        {
            std::cout << "No Physical Device matched the requirments.";
            return;
        }


        std::vector<vk::QueueFamilyProperties> queueFamilies = m_physicalDevice.getQueueFamilyProperties();

        for (unsigned int i = 0; i < queueFamilies.size(); i++)
        {
            if (graphicsQueue.m_type == Queue::Type::None && queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                graphicsQueue.m_type = Queue::Type::Graphics;
                graphicsQueue.m_index = i;
            }
            if (computeQueue.m_type == Queue::Type::None && queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute)
            {
                computeQueue.m_type = Queue::Type::Compute;
                computeQueue.m_index = i;
            }
            if (transferQueue.m_type == Queue::Type::None && queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer)
            {
                transferQueue.m_type = Queue::Type::Transfer;
                transferQueue.m_index = i;
            }
            if (surface != nullptr && presentQueue.m_type == Queue::Type::None && m_physicalDevice.getSurfaceSupportKHR(i, surface))
            {
                presentQueue.m_type = Queue::Type::Transfer;
                presentQueue.m_index = i;
            }

            if ((int) graphicsQueue.m_type + (int) computeQueue.m_type + (int) presentQueue.m_type + (int) transferQueue.m_type == 0) break;
        }

        std::set<unsigned int> uniqueQueueFamilies;
        if (graphicsQueue.m_type != Queue::Type::None) uniqueQueueFamilies.insert(graphicsQueue.GetIndex());
        if (computeQueue.m_type != Queue::Type::None) uniqueQueueFamilies.insert(computeQueue.GetIndex());
        if (presentQueue.m_type != Queue::Type::None) uniqueQueueFamilies.insert(presentQueue.GetIndex());
        if (transferQueue.m_type != Queue::Type::None) uniqueQueueFamilies.insert(transferQueue.GetIndex());

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        float priority = 0;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, queueFamily, 1, &priority));
        }

        vk::DeviceQueueCreateInfo();

        std::vector<const char*> extensionsConstChar;
        for (const auto& extension : extensions)extensionsConstChar.push_back(extension.data());

        vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), queueCreateInfos, nullptr, extensionsConstChar);
        m_handle = m_physicalDevice.createDevice(createInfo);

        graphicsQueue.m_handle = graphicsQueue.m_type == Queue::Type::None ? nullptr : m_handle.getQueue(graphicsQueue.GetIndex(), 0);
        computeQueue.m_handle = computeQueue.m_type == Queue::Type::None ? nullptr : m_handle.getQueue(computeQueue.GetIndex(), 0);
        transferQueue.m_handle = transferQueue.m_type == Queue::Type::None ? nullptr : m_handle.getQueue(transferQueue.GetIndex(), 0);
        presentQueue.m_handle = presentQueue.m_type == Queue::Type::None ? nullptr : m_handle.getQueue(presentQueue.GetIndex(), 0);
        graphicsQueue.m_device = m_handle;
        computeQueue.m_device = m_handle;
        transferQueue.m_device = m_handle;
        presentQueue.m_device = m_handle;
        graphicsQueue.m_commandPool = m_handle.createCommandPool({ vk::CommandPoolCreateFlagBits::eResetCommandBuffer });
        computeQueue.m_commandPool = m_handle.createCommandPool({ vk::CommandPoolCreateFlagBits::eResetCommandBuffer });
        transferQueue.m_commandPool = m_handle.createCommandPool({ vk::CommandPoolCreateFlagBits::eResetCommandBuffer });
        presentQueue.m_commandPool = m_handle.createCommandPool({ vk::CommandPoolCreateFlagBits::eResetCommandBuffer });
    }

    Device::Device() : m_handle(nullptr), m_physicalDevice(nullptr) {}

    Device::Device(Device&& other) noexcept
        :m_handle(other.m_handle), m_physicalDevice(other.m_physicalDevice)
    {
        other.m_handle = nullptr;
        m_physicalDevice = nullptr;
    }

    Device::~Device()
    {
        if (m_handle == nullptr) return;
        graphicsQueue.~Queue();
        computeQueue.~Queue();
        presentQueue.~Queue();
        transferQueue.~Queue();

        vkDestroyDevice(m_handle, nullptr);
    }

    Device& Device::operator=(Device&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_physicalDevice, other.m_physicalDevice);

        return *this;
    }

    Device::operator const DeviceHandle& () const
    {
        return m_handle;
    }

    Device::operator const PhysicalDeviceHandle& () const
    {
        return m_physicalDevice;
    }
    Device::operator DeviceHandle& ()
    {
        return m_handle;
    }

    Device::operator PhysicalDeviceHandle& ()
    {
        return m_physicalDevice;
    }

    void Device::WaitUntilIdle()
    {
        vkDeviceWaitIdle(m_handle);
    }
}