#include <vulkan/vulkan.h>
#include "Queue.h"

namespace vg
{
    Queue::Queue() :m_handle(nullptr), m_type(Type::None), m_index(0) {}

    Queue::Queue(QueueHandle handle, Type type, unsigned int index) : m_handle(handle), m_type(type), m_index(index) {}

    Queue::Queue(Queue&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_type, other.m_type);
        other.m_handle = nullptr;
    }

    Queue& Queue::operator=(Queue&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_type, other.m_type);
        other.m_handle = nullptr;

        return *this;
    }

    Queue::Type Queue::GetType() const
    {
        return m_type;
    }

    unsigned int Queue::GetIndex() const
    {
        return m_index;
    }

    Queue::operator QueueHandle() const
    {
        return m_handle;
    }
}