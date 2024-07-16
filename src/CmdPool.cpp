#include <vulkan/vulkan.hpp>
#include "CmdPool.h"
#include "Device.h"

namespace vg
{
    CmdPool::CmdPool(const class Queue& queue, Flags<CmdPoolUsage> usage) : m_queue(queue)
    {
        m_handle = ((DeviceHandle) currentDevice).createCommandPool({ (vk::CommandPoolCreateFlags) usage, queue.GetIndex() });
    }

    CmdPool::CmdPool() :m_handle(nullptr), m_queue(nullptr)
    {}

    CmdPool::~CmdPool()
    {
        if (m_handle == nullptr)
            return;

        ((DeviceHandle) currentDevice).destroyCommandPool(m_handle);
        m_handle = nullptr;
    }

    CmdPool::CmdPool(CmdPool&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_queue, other.m_queue);
    }

    CmdPool& CmdPool::operator=(CmdPool&& other) noexcept
    {
        if (&other == this)
            return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_queue, other.m_queue);

        return *this;
    }

    CmdPool::operator const CmdPoolHandle& () const
    {
        return m_handle;
    }

    const QueueHandle CmdPool::GetQueue() const
    {
        return m_queue;
    }

    void CmdPool::Trim()
    {
        ((DeviceHandle) currentDevice).trimCommandPool(m_handle, (vk::CommandPoolTrimFlags) 0);
    }

    void CmdPool::Reset(bool releaseResources)
    {
        ((DeviceHandle) currentDevice).resetCommandPool(m_handle, (vk::CommandPoolResetFlags) (releaseResources ? (int) vk::CommandPoolResetFlagBits::eReleaseResources : 0));
    }
}