#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>
#include <iostream>
#include "Device.h"

namespace vg
{
    Device* currentDevice;
    int DefaultScoreFunction(
        const std::vector<Queue*>& queues,
        const std::set<std::string>& extensions,
        const std::set<std::string>& supportedExtensions,
        DeviceType type,
        const DeviceLimits& limits,
        const DeviceFeatures& features)
    {
        for (const auto& queue : queues)
            if (queue == nullptr) return -1;

        for (const auto& extension : extensions)
            if (!supportedExtensions.contains(extension)) return -1;

        return 0;
    }

    Device::Device(
        Span<Queue* const> queues,
        const std::set<std::string>& extensions,
        const DeviceFeatures& hintedDeviceEnabledFeatures,
        SurfaceHandle surface,
        std::function<int(PhysicalDeviceHandle id, Span<Queue* const> supportedQueues, const std::set<std::string>& supportedExtensions, DeviceType type, const DeviceLimits& limits, const DeviceFeatures& features)> scoreFunction
    ) :m_queues(queues.begin(), queues.end())
    {
        assert(queues.size() > 0);
        bool hasPresentQueueType = false;
        for (auto&& queue : queues)
        {
            if (queue->GetType().IsSet(QueueType::Present))
            {
                hasPresentQueueType = true;
                break;
            }
        }
        assert(hasPresentQueueType ^ (surface == nullptr));

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

            // Check what queues are supported
            std::vector<Queue*> supportedQueues(queues.begin(), queues.end());
            auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            for (unsigned int i = 0; i < supportedQueues.size(); i++)
            {
                unsigned int minDifference = -1;
                unsigned int bestIndex = -1;
                for (unsigned int j = 0; j < queueFamilies.size(); j++)
                {
                    // Check if there are any queues available left.
                    if (queueFamilies[j].queueCount == 0) continue;

                    // Get the queueFamily type.
                    Flags<QueueType> type;
                    if (queueFamilies[j].queueFlags & vk::QueueFlagBits::eGraphics) type.Set(QueueType::Graphics);
                    if (queueFamilies[j].queueFlags & vk::QueueFlagBits::eCompute) type.Set(QueueType::Compute);
                    if (queueFamilies[j].queueFlags & vk::QueueFlagBits::eTransfer) type.Set(QueueType::Transfer);
                    if (surface != nullptr && physicalDevice.getSurfaceSupportKHR(j, surface)) type.Set(QueueType::Present);

                    // Check if family has at least all queue types required by queue.
                    if ((supportedQueues[i]->m_type & type) != supportedQueues[i]->m_type)
                        continue;

                    // Count how many queue types are not needed.
                    type ^= supportedQueues[i]->m_type;
                    unsigned int difference = type.GetSetCount();

                    // Update best index.
                    if (difference < minDifference)
                    {
                        bestIndex = j;
                        minDifference = difference;
                    }
                }

                if (bestIndex == -1)
                    supportedQueues[i] = nullptr;
                else
                    queueFamilies[bestIndex].queueCount--;
            }

            // Get DeviceType, it's limits and features.
            DeviceType type = (DeviceType) physicalDevice.getProperties().deviceType;
            auto limits = physicalDevice.getProperties().limits;
            auto features = physicalDevice.getFeatures();

            // Score the device.
            int score;
            if (scoreFunction != nullptr)
                score = scoreFunction(physicalDevice, supportedQueues, supportedExtensions, type, *(DeviceLimits*) &limits, *(DeviceFeatures*) &features);

            else
                score = DefaultScoreFunction(supportedQueues, extensions, supportedExtensions, type, *(DeviceLimits*) &limits, *(DeviceFeatures*) &features);

            if (score > highestScore)
            {
                m_physicalDevice = physicalDevice;
                highestScore = score;
            }
        }
        if (highestScore == -1)
            std::runtime_error("No Physical Device matched the requirments.");

        // Pick queue families.
        std::vector<vk::QueueFamilyProperties> queueFamilies = m_physicalDevice.getQueueFamilyProperties();
        std::map<unsigned int, std::vector<float>> queueFamilyPriorities;
        for (unsigned int i = 0; i < queues.size(); i++)
        {
            unsigned int minDifference = -1;
            unsigned int bestIndex = -1;
            for (unsigned int j = 0; j < queueFamilies.size(); j++)
            {
                // Get the queueFamily type.
                Flags<QueueType> type;
                if (queueFamilies[j].queueFlags & vk::QueueFlagBits::eGraphics) type.Set(QueueType::Graphics);
                if (queueFamilies[j].queueFlags & vk::QueueFlagBits::eCompute) type.Set(QueueType::Compute);
                if (queueFamilies[j].queueFlags & vk::QueueFlagBits::eTransfer) type.Set(QueueType::Transfer);
                if (surface != nullptr && m_physicalDevice.getSurfaceSupportKHR(j, surface)) type.Set(QueueType::Present);

                // Check if family has at least all queue types required by queue.
                if ((queues[i]->m_type & type) != queues[i]->m_type)
                    continue;

                // Count how many queue types are not needed.
                type ^= queues[i]->m_type;
                unsigned int difference = type.GetSetCount();

                // Update best index.
                if (difference < minDifference)
                {
                    bestIndex = j;
                    minDifference = difference;
                }
            }

            queues[i]->m_index = bestIndex;

            if (queues[i]->m_index == -1)
                queues[i]->m_type = QueueType::None;
            else
            {
                if (queueFamilyPriorities[queues[i]->GetIndex()].size() < queueFamilies[queues[i]->GetIndex()].queueCount)
                    queueFamilyPriorities[queues[i]->GetIndex()].push_back(queues[i]->m_priority);
            }
        }

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        for (auto&& [index, priorities] : queueFamilyPriorities)
            queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, index, priorities.size(), priorities.data()));

        std::vector<const char*> extensionsConstChar;
        for (const auto& extension : extensions)extensionsConstChar.push_back(extension.data());

        auto features_ = m_physicalDevice.getFeatures();
        DeviceFeatures features = *(DeviceFeatures*) &features_;
        features &= hintedDeviceEnabledFeatures;

        vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), queueCreateInfos, nullptr, extensionsConstChar, (vk::PhysicalDeviceFeatures*) &features);
        m_handle = m_physicalDevice.createDevice(createInfo);

        SCOPED_DEVICE_CHANGE(this);

        std::map<unsigned int, unsigned int> queueIndices;
        std::map<unsigned int, std::vector<Queue*>> queuePointers;
        for (auto&& queue : queues)
        {
            if (queue->GetType() == QueueType::None) continue;
            if (queueIndices.contains(queue->GetIndex()))
                queueIndices[queue->GetIndex()]++;
            else
                queueIndices[queue->GetIndex()] = 1;

            unsigned int maxIndex = queueFamilies[queue->GetIndex()].queueCount;
            unsigned int queueIndex = queueIndices[queue->GetIndex()] - 1;
            if (queueIndex >= maxIndex)
            {
                queueIndex = queueIndex % maxIndex;
                queue->m_type = QueueType::None;
                queue->m_handle = queuePointers[queue->GetIndex()][queueIndex]->m_handle;
                queue->m_commandPool = queuePointers[queue->GetIndex()][queueIndex]->m_commandPool;
                queue->m_transientCommandPool = queuePointers[queue->GetIndex()][queueIndex]->m_transientCommandPool;
            }
            else
            {
                queue->m_handle = m_handle.getQueue(queue->GetIndex(), queueIndex);
                queue->m_commandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer },queue->GetIndex() });
                queue->m_transientCommandPool = m_handle.createCommandPool({ { vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient },queue->GetIndex() });
                queuePointers[queue->GetIndex()].push_back(queue);
            }
        }
    }

    Device::Device() : m_handle(nullptr), m_physicalDevice(nullptr), m_queues{} {}

    Device::Device(Device&& other) noexcept
        : Device()
    {
        *this = std::move(other);
    }

    Device::~Device()
    {
        if (m_handle == nullptr) return;

        SCOPED_DEVICE_CHANGE(this);
        for (auto&& queue : m_queues)
        {
            ((DeviceHandle) *currentDevice).destroyCommandPool(queue->m_commandPool);
            ((DeviceHandle) *currentDevice).destroyCommandPool(queue->m_transientCommandPool);
        }

        vkDestroyDevice(m_handle, nullptr);
        m_queues.clear();
        m_queues.shrink_to_fit();
        m_handle = nullptr;
    }

    Device& Device::operator=(Device&& other) noexcept
    {
        if (&other == this) return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_physicalDevice, other.m_physicalDevice);
        std::swap(m_queues, other.m_queues);

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

    std::set<std::string> Device::GetExtensions() const
    {
        std::set<std::string> supportedExtensions;
        for (const auto& extension : m_physicalDevice.enumerateDeviceExtensionProperties())
            supportedExtensions.insert(extension.extensionName);

        return supportedExtensions;
    }

    DeviceLimits Device::GetLimits() const
    {
        return GetProperties().limits;
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

    const Queue& Device::GetQueue(uint32_t queueIndex) const
    {
        return *m_queues[queueIndex];
    }
}