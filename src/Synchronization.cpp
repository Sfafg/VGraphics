#include <vulkan/vulkan.hpp>
#include "Synchronization.h"
namespace vg
{
    Fence::Fence(bool createSignalled)
    {
        m_handle = ((DeviceHandle) *currentDevice).createFence({ createSignalled ? vk::FenceCreateFlagBits::eSignaled : (vk::FenceCreateFlagBits) 0U });
    }

    Fence::Fence(void* ptr) : m_handle(nullptr) { assert(ptr == nullptr); }
    Fence::Fence(Fence&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
    }

    Fence::~Fence()
    {
        if (m_handle == nullptr) return;
        ((DeviceHandle) *currentDevice).destroyFence(m_handle);
    }

    Fence& Fence::operator=(Fence&& other) noexcept
    {
        if (this == &other) return *this;
        std::swap(m_handle, other.m_handle);


        return *this;
    }

    Fence::operator const FenceHandle& () const
    {
        return m_handle;
    }

    bool Fence::IsSignaled() const
    {
        return ((DeviceHandle) *currentDevice).getFenceStatus(m_handle) == vk::Result::eSuccess;
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
        auto result = ((DeviceHandle) *currentDevice).waitForFences(*(std::vector<vk::Fence>*) & fenceHandles, true, timeout);
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
        auto result = ((DeviceHandle) *currentDevice).waitForFences(*(std::vector<vk::Fence>*) & fenceHandles, false, timeout);
    }

    void Fence::ResetAll(const std::vector<std::reference_wrapper<Fence>>& fences)
    {
        std::vector<FenceHandle> fenceHandles;
        fenceHandles.reserve(fences.size());
        for (int i = 0; i < fences.size(); i++) fenceHandles.push_back(fences[i].get());
        ((DeviceHandle) *currentDevice).resetFences(*(std::vector<vk::Fence>*) & fenceHandles);
    }

    Semaphore::Semaphore()
    {
        m_handle = ((DeviceHandle) *currentDevice).createSemaphore({});
    }

    Semaphore::Semaphore(void* ptr) : m_handle(nullptr) { assert(ptr == nullptr); }
    Semaphore::Semaphore(Semaphore&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);

    }

    Semaphore::~Semaphore()
    {
        if (m_handle == nullptr) return;
        vkDestroySemaphore((DeviceHandle) *currentDevice, m_handle, nullptr);
    }

    Semaphore& Semaphore::operator=(Semaphore&& other) noexcept
    {
        if (this == &other) return *this;
        std::swap(m_handle, other.m_handle);


        return *this;
    }

    Semaphore::operator const SemaphoreHandle& () const
    {
        return m_handle;
    }
}