#include <vulkan/vulkan.hpp>
#include <vector>
#include "Device.h"

namespace vg
{
    bool IsNone(const Queue& queue) { return queue.GetType() == Queue::Type::None; }
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

    Device::Device() : m_handle(nullptr), m_physicalDevice(nullptr) {}

    Device::Device(
        const std::set<Queue::Type>& queues,
        const std::set<std::string>& extensions,
        SurfaceHandle surface,
        std::function<int(const std::set<Queue::Type>& supportedQueues, const std::set<std::string>& supportedExtensions, Type type, const Limits& limits)> scoreFunction
    )
    {
        assert(queues.size() > 0);
        assert(queues.contains(Queue::Type::Present) ^ surface == nullptr);

        // Pick physical device.
        int highestScore = -1;
        for (const auto& physicalDevice : vk::Instance((InstanceHandle) vg::instance).enumeratePhysicalDevices())
        {
            std::set<std::string> supportedExtensions;
            for (const auto& extension : physicalDevice.enumerateDeviceExtensionProperties())
            {
                supportedExtensions.insert(extension.extensionName);
            }
            if (surface != nullptr && extensions.contains("VK_KHR_swapchain") && supportedExtensions.contains("VK_KHR_swapchain"))
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

                if (supportedQueues.size() == 3 + (surface != nullptr)) break;
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


        std::vector<vk::QueueFamilyProperties> queueFamilies = vk::PhysicalDevice(m_physicalDevice).getQueueFamilyProperties();

        for (unsigned int i = 0; i < queueFamilies.size(); i++)
        {
            if (IsNone(graphicsQueue) && queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)  graphicsQueue = Queue(vk::Queue(nullptr), Queue::Type::Graphics, i);
            if (IsNone(computeQueue) && queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute)  computeQueue = Queue(vk::Queue(nullptr), Queue::Type::Compute, i);
            if (IsNone(transferQueue) && queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer)  transferQueue = Queue(vk::Queue(nullptr), Queue::Type::Transfer, i);
            if (surface != nullptr && IsNone(presentQueue) && vk::PhysicalDevice(m_physicalDevice).getSurfaceSupportKHR(i, surface))  presentQueue = Queue(vk::Queue(nullptr), Queue::Type::Present, i);

            if (!IsNone(graphicsQueue) && !IsNone(computeQueue) && !IsNone(presentQueue) && !IsNone(transferQueue)) break;
        }

        std::set<unsigned int> uniqueQueueFamilies;
        if (!IsNone(graphicsQueue)) uniqueQueueFamilies.insert(graphicsQueue.GetIndex());
        if (!IsNone(computeQueue)) uniqueQueueFamilies.insert(computeQueue.GetIndex());
        if (!IsNone(presentQueue)) uniqueQueueFamilies.insert(presentQueue.GetIndex());
        if (!IsNone(transferQueue)) uniqueQueueFamilies.insert(transferQueue.GetIndex());

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        float priority = 0;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, queueFamily, 1, &priority));
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        std::vector<const char*> extensionsConstChar;
        for (const auto& extension : extensions)extensionsConstChar.push_back(extension.data());

        vk::DeviceCreateInfo createInfo(vk::DeviceCreateFlags(), queueCreateInfos, nullptr, extensionsConstChar);
        m_handle = vk::PhysicalDevice((PhysicalDeviceHandle) m_physicalDevice).createDevice(createInfo);

        graphicsQueue = Queue((IsNone(graphicsQueue)) ? vk::Queue(nullptr) : vk::Device(m_handle).getQueue(graphicsQueue.GetIndex(), 0), graphicsQueue.GetType(), graphicsQueue.GetIndex());
        computeQueue = Queue((IsNone(computeQueue)) ? vk::Queue(nullptr) : vk::Device(m_handle).getQueue(computeQueue.GetIndex(), 0), computeQueue.GetType(), computeQueue.GetIndex());
        transferQueue = Queue((IsNone(transferQueue)) ? vk::Queue(nullptr) : vk::Device(m_handle).getQueue(transferQueue.GetIndex(), 0), transferQueue.GetType(), transferQueue.GetIndex());
        presentQueue = Queue((IsNone(presentQueue)) ? vk::Queue(nullptr) : vk::Device(m_handle).getQueue(presentQueue.GetIndex(), 0), presentQueue.GetType(), presentQueue.GetIndex());
    }

    Device::Device(Device&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_physicalDevice, other.m_physicalDevice);
        other.m_handle = nullptr;
    }

    Device& Device::operator=(Device&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_physicalDevice, other.m_physicalDevice);
        other.m_handle = nullptr;

        return *this;
    }

    Device::~Device()
    {
        if (m_handle == nullptr) return;
        vkDestroyDevice(m_handle, nullptr);
    }

    Device::operator DeviceHandle() const
    {
        return m_handle;
    }

    Device::operator PhysicalDeviceHandle() const
    {
        return m_physicalDevice;
    }
}