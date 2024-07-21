#include <vulkan/vulkan.hpp>
#include <vector>
#include <iostream>
#include "Device.h"

namespace vg
{
    Device currentDevice;
    int DefaultScoreFunction(
        const std::set<QueueType>& queues,
        const std::set<std::string>& extensions,
        const std::set<QueueType>& supportedQueues,
        const std::set<std::string>& supportedExtensions,
        DeviceType type,
        const DeviceLimits& limits,
        const DeviceFeatures& features)
    {
        for (const auto& queue : queues)
            if (!supportedQueues.contains(queue)) return -1;

        for (const auto& extension : extensions)
            if (!supportedExtensions.contains(extension)) return -1;

        return 0;
    }

    Device::Device(
        const std::set<QueueType>& queues,
        const std::set<std::string>& extensions,
        const DeviceFeatures& hintedDeviceEnabledFeatures,
        SurfaceHandle surface,
        std::function<int(const std::set<QueueType>& supportedQueues, const std::set<std::string>& supportedExtensions, DeviceType type, const DeviceLimits& limits, const DeviceFeatures& features)> scoreFunction
    )
    {
        assert(queues.size() > 0);
        assert(queues.contains(QueueType::Present) ^ (surface == nullptr));

        // Pick physical device.
        int highestScore = -1;
        for (const auto& physicalDevice : ((InstanceHandle) vg::instance).enumeratePhysicalDevices())
        {
            // Get Supported Extensions.
            std::set<std::string> supportedExtensions;
            for (const auto& extension : physicalDevice.enumerateDeviceExtensionProperties())
            {
                supportedExtensions.insert(extension.extensionName);
            }

            // If surface is specified then check if this device supports it.
            if (surface != nullptr && extensions.contains("VK_KHR_swapchain") && supportedExtensions.contains("VK_KHR_win32_surface"))
            {
                unsigned int formatCount, presentModeCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
                vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
                if (formatCount == 0 || presentModeCount == 0) continue;
            }

            // Get Supported Queues.
            std::set<QueueType> supportedQueues;
            auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            for (unsigned int i = 0; i < queueFamilies.size(); i++)
            {
                if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)supportedQueues.insert(QueueType::Graphics);
                if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) supportedQueues.insert(QueueType::Compute);
                if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer)supportedQueues.insert(QueueType::Transfer);
                if (surface != nullptr && physicalDevice.getSurfaceSupportKHR(i, surface)) supportedQueues.insert(QueueType::Present);

                if ((int) supportedQueues.size() == 3 + (surface != nullptr)) break;
            }

            // Get DeviceType and it's limits.
            DeviceType type = (DeviceType) physicalDevice.getProperties().deviceType;
            auto limits = physicalDevice.getProperties().limits;
            auto features = physicalDevice.getFeatures();

            // Score the
            int score;
            if (scoreFunction != nullptr)
                score = scoreFunction(supportedQueues, supportedExtensions, type, *(DeviceLimits*) &limits, *(DeviceFeatures*) &features);

            else
                score = DefaultScoreFunction(queues, extensions, supportedQueues, supportedExtensions, type, *(DeviceLimits*) &limits, *(DeviceFeatures*) &features);

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

        // Pick queue families.
        std::vector<vk::QueueFamilyProperties> queueFamilies = m_physicalDevice.getQueueFamilyProperties();

        for (unsigned int i = 0; i < queueFamilies.size(); i++)
        {
            int pickCount = 0;
            if (graphicsQueue.m_type == QueueType::None && queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                graphicsQueue.m_type = QueueType::Graphics;
                graphicsQueue.m_index = i;
                pickCount++;
            }

            if (surface != nullptr && presentQueue.m_type == QueueType::None && m_physicalDevice.getSurfaceSupportKHR(i, surface))
            {
                presentQueue.m_type = QueueType::Present;
                presentQueue.m_index = i;
                pickCount++;
            }

            if (transferQueue.m_type == QueueType::None && queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer)
            {
                transferQueue.m_type = QueueType::Transfer;
                transferQueue.m_index = i;
                pickCount++;
            }

            if (computeQueue.m_type == QueueType::None && queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute)
            {
                if (pickCount == 0)
                    computeQueue.m_type = QueueType::Compute;
                computeQueue.m_index = i;
            }



            if ((int) graphicsQueue.m_type + (int) computeQueue.m_type + (int) presentQueue.m_type + (int) transferQueue.m_type == 0) break;
        }
        computeQueue.m_type = QueueType::Compute;

        std::set<unsigned int> uniqueQueueFamilies;
        if (graphicsQueue.m_type != QueueType::None) uniqueQueueFamilies.insert(graphicsQueue.GetIndex());
        if (computeQueue.m_type != QueueType::None) uniqueQueueFamilies.insert(computeQueue.GetIndex());
        if (presentQueue.m_type != QueueType::None) uniqueQueueFamilies.insert(presentQueue.GetIndex());
        if (transferQueue.m_type != QueueType::None) uniqueQueueFamilies.insert(transferQueue.GetIndex());

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        float priority = 0;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, queueFamily, 1, &priority));
        }

        vk::DeviceQueueCreateInfo();

        std::vector<const char*> extensionsConstChar;
        for (const auto& extension : extensions)extensionsConstChar.push_back(extension.data());

        vk::PhysicalDeviceFeatures features = m_physicalDevice.getFeatures();
        features = (*(DeviceFeatures*) &features) & hintedDeviceEnabledFeatures;
        vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), queueCreateInfos, nullptr, extensionsConstChar, &features);
        m_handle = m_physicalDevice.createDevice(createInfo);

        auto prevCurrentHandle = currentDevice.m_handle;
        currentDevice.m_handle = m_handle;

        graphicsQueue.m_handle = graphicsQueue.m_type == QueueType::None ? nullptr : m_handle.getQueue(graphicsQueue.GetIndex(), 0);
        computeQueue.m_handle = computeQueue.m_type == QueueType::None ? nullptr : m_handle.getQueue(computeQueue.GetIndex(), 0);
        transferQueue.m_handle = transferQueue.m_type == QueueType::None ? nullptr : m_handle.getQueue(transferQueue.GetIndex(), 0);
        presentQueue.m_handle = presentQueue.m_type == QueueType::None ? nullptr : m_handle.getQueue(presentQueue.GetIndex(), 0);

        graphicsQueue.m_commandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer },graphicsQueue.GetIndex() });
        computeQueue.m_commandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer },computeQueue.GetIndex() });
        transferQueue.m_commandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer },transferQueue.GetIndex() });
        presentQueue.m_commandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer },presentQueue.GetIndex() });

        graphicsQueue.m_transientCommandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient },graphicsQueue.GetIndex() });
        computeQueue.m_transientCommandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient },computeQueue.GetIndex() });
        transferQueue.m_transientCommandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient },transferQueue.GetIndex() });
        presentQueue.m_transientCommandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient },presentQueue.GetIndex() });

        currentDevice.m_handle = prevCurrentHandle;
    }

    Device::Device() : m_handle(nullptr), m_physicalDevice(nullptr) {}

    Device::Device(Device&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_physicalDevice, other.m_physicalDevice);
        std::swap(graphicsQueue, other.graphicsQueue);
        std::swap(computeQueue, other.computeQueue);
        std::swap(presentQueue, other.presentQueue);
        std::swap(transferQueue, other.transferQueue);
    }

    Device::~Device()
    {
        if (m_handle == nullptr) return;
        graphicsQueue.~Queue();
        computeQueue.~Queue();
        presentQueue.~Queue();
        transferQueue.~Queue();

        vkDestroyDevice(m_handle, nullptr);
        m_handle = nullptr;
    }

    Device& Device::operator=(Device&& other) noexcept
    {
        if (&other == this) return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_physicalDevice, other.m_physicalDevice);
        std::swap(graphicsQueue, other.graphicsQueue);
        std::swap(computeQueue, other.computeQueue);
        std::swap(presentQueue, other.presentQueue);
        std::swap(transferQueue, other.transferQueue);

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

    void Device::WaitUntilIdle()
    {
        vkDeviceWaitIdle(m_handle);
    }

    DeviceProperties Device::GetProperties() const
    {
        auto properties = m_physicalDevice.getProperties();
        return *(DeviceProperties*) &properties;
    }

    DeviceFeatures Device::GetFeatures() const
    {
        auto features = m_physicalDevice.getFeatures();
        return *(DeviceFeatures*) &features;
    }

    FormatProperties Device::GetFormatProperties(Format format) const
    {
        auto properties = m_physicalDevice.getFormatProperties((vk::Format) format);
        return *(FormatProperties*) &properties;
    }


}