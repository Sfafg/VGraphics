#include <vulkan/vulkan.hpp>
#include "Synchronization.h"

namespace vg
{
    Fence::Fence(DeviceHandle device, bool createSignalled) : m_device(device)
    {
        m_handle = m_device.createFence({ createSignalled ? vk::FenceCreateFlagBits::eSignaled : (vk::FenceCreateFlagBits) 0U });
    }

    Fence::Fence() : m_handle(nullptr), m_device(nullptr) {}

    Fence::Fence(Fence&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);
    }

    Fence::~Fence()
    {
        if (m_handle == nullptr) return;
        m_device.destroyFence(m_handle);
    }

    Fence& Fence::operator=(Fence&& other) noexcept
    {
        if (this == &other) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);

        return *this;
    }

    Fence::operator const FenceHandle& () const
    {
        return m_handle;
    }

    DeviceHandle Fence::GetDevice() const
    {
        return m_device;
    }

    bool Fence::IsSignaled() const
    {
        return m_device.getFenceStatus(m_handle) == vk::Result::eSuccess;
    }

    void Fence::Await(bool reset, uint64_t timeout)
    {
        Fence::AwaitAll({ *this }, reset, timeout);
    }

    void Fence::Reset()
    {
        Fence::ResetAll({ *this });
    }

    void Fence::AwaitAll(const std::vector<std::reference_wrapper<Fence>>& fences, bool reset, uint64_t timeout)
    {
        std::vector<FenceHandle> fenceHandles;
        fenceHandles.reserve(fences.size());
        for (int i = 0; i < fences.size(); i++) fenceHandles.push_back(fences[i].get());
        auto result = fences[0].get().GetDevice().waitForFences(*(std::vector<vk::Fence>*) & fenceHandles, true, timeout);
        if (reset)
        {
            Fence::ResetAll(fences);
        }
    }

    void Fence::AwaitAny(const std::vector<std::reference_wrapper<Fence>>& fences, uint64_t timeout)
    {
        std::vector<FenceHandle> fenceHandles;
        fenceHandles.reserve(fences.size());
        for (int i = 0; i < fences.size(); i++) fenceHandles.push_back(fences[i].get());
        auto result = fences[0].get().GetDevice().waitForFences(*(std::vector<vk::Fence>*) & fenceHandles, false, timeout);
    }

    void Fence::ResetAll(const std::vector<std::reference_wrapper<Fence>>& fences)
    {
        std::vector<FenceHandle> fenceHandles;
        fenceHandles.reserve(fences.size());
        for (int i = 0; i < fences.size(); i++) fenceHandles.push_back(fences[i].get());
        fences[0].get().GetDevice().resetFences(*(std::vector<vk::Fence>*) & fenceHandles);
    }

    Semaphore::Semaphore(DeviceHandle device) : m_device(device)
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