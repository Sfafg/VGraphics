#include <vulkan/vulkan.hpp>
#include "Semaphore.h"

namespace vg
{
    Semaphore::Semaphore(const Device& device) : m_device(device)
    {
        m_handle = m_device.createSemaphore({});
    }

    Semaphore::Semaphore() : m_handle(nullptr), m_device(nullptr) {}

    Semaphore::Semaphore(Semaphore&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);
    }

    Semaphore::~Semaphore()
    {
        if (m_handle == nullptr) return;
        vkDestroySemaphore(m_device, m_handle, nullptr);
    }

    Semaphore& Semaphore::operator=(Semaphore&& other) noexcept
    {
        if (this == &other) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);

        return *this;
    }

    Semaphore::operator const SemaphoreHandle& () const
    {
        return m_handle;
    }
}