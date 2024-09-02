#include <vulkan/vulkan.hpp>
#include <iostream>
#include "Queue.h"
#include "Structs.h"
#include "Synchronization.h"

namespace vg
{
    Queue::Queue(Flags<QueueType> type, float priority) :m_handle(nullptr), m_type(type), m_priority(priority), m_index(0) {}

    Queue::Queue() :m_handle(nullptr), m_type(0), m_priority(0), m_index(0) {}

    Queue::Queue(Queue&& other) noexcept
        :Queue()
    {
        *this = std::move(other);
    }

    Queue& Queue::operator=(Queue&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_commandPool, other.m_commandPool);
        std::swap(m_transientCommandPool, other.m_transientCommandPool);
        std::swap(m_type, other.m_type);
        std::swap(m_priority, other.m_priority);
        std::swap(m_index, other.m_index);

        return *this;
    }

    Queue::operator const QueueHandle& () const
    {
        return m_handle;
    }

    Flags<QueueType> Queue::GetType() const
    {
        return m_type;
    }

    unsigned int Queue::GetIndex() const
    {
        return m_index;
    }

    CmdPoolHandle Queue::GetCmdPool(bool transient) const
    {
        if (transient)
            return m_transientCommandPool;
        return m_commandPool;
    }

    Result Queue::Present(Span<const SemaphoreHandle> waitSemaphores, Span<const SwapchainHandle> swapchains, Span<const uint32_t> imageIndices)
    {
        vk::PresentInfoKHR info(*(Span<const vk::Semaphore>*) & waitSemaphores, *(Span<const vk::SwapchainKHR>*) & swapchains, imageIndices);
        return (Result) vkQueuePresentKHR(m_handle, (VkPresentInfoKHR*) &info);
    }

    void Queue::Submit(Span<const SubmitInfo> submits, const Fence& fence)
    {
        m_handle.submit(*(Span<const vk::SubmitInfo>*) & submits, fence);
    }

    Fence Queue::Submit(Span<const class SubmitInfo> submits)
    {
        Fence fence;
        Submit(submits, fence);

        return fence;
    }

}