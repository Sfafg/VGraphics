#include <vulkan/vulkan.hpp>
#include "Queue.h"
#include "Synchronization.h"

namespace vg
{
    Queue::Queue() :m_handle(nullptr), m_commandPool(nullptr), m_type(Type::None), m_index(0) {}

    Queue::Queue(Queue&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_commandPool, other.m_commandPool);
        std::swap(m_type, other.m_type);
        std::swap(m_index, other.m_index);

        other.m_handle = nullptr;
    }
    Queue::~Queue()
    {
        if (m_commandPool == nullptr)return;
        ((DeviceHandle) currentDevice).destroyCommandPool(m_commandPool);
        m_commandPool = nullptr;
    }
    Queue& Queue::operator=(Queue&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_commandPool, other.m_commandPool);
        std::swap(m_type, other.m_type);
        std::swap(m_index, other.m_index);

        return *this;
    }

    Queue::operator const QueueHandle& () const
    {
        return m_handle;
    }

    Queue::Type Queue::GetType() const
    {
        return m_type;
    }

    unsigned int Queue::GetIndex() const
    {
        return m_index;
    }

    CommandPoolHandle Queue::GetCommandPool() const
    {
        return m_commandPool;
    }

    void Queue::Present(const std::vector<SemaphoreHandle>& waitSemaphores, const std::vector<SwapchainHandle>& swapchains, const std::vector<uint32_t>& imageIndices)
    {
        auto result = m_handle.presentKHR({ *(std::vector<vk::Semaphore>*) & waitSemaphores,*(std::vector<vk::SwapchainKHR>*) & swapchains,imageIndices });
    }
}