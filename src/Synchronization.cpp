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

    /// @brief Await untill fence enters the signalled state.
    void Fence::Await(bool reset, uint64_t timeout)
    {
        Fence::AwaitAll(std::vector<const Fence*>{ this }, reset, timeout);
    }

    /// @brief  Make Fence Unsignalled
    void Fence::Reset()
    {
        Fence::ResetAll(std::vector<const Fence*>{ this });
    }

    /// @brief Await untill all fence enter the signalled state.
    /// @param fences 
    /// @param timeout 
    void Fence::AwaitAll(const std::vector<const Fence*>& fences, bool reset, uint64_t timeout)
    {
        std::vector<FenceHandle> fenceHandles;
        fenceHandles.reserve(fences.size());
        for (int i = 0; i < fences.size(); i++) fenceHandles.push_back(*fences[i]);
        auto result = ((DeviceHandle) *currentDevice).waitForFences(*(std::vector<vk::Fence>*) & fenceHandles, true, timeout);
        if (reset)
        {
            Fence::ResetAll(fences);
        }
    }

    /// @brief Await untill any fence enter the signalled state.
    /// @param fences 
    /// @param timeout 
    void Fence::AwaitAny(const std::vector<const Fence*>& fences, uint64_t timeout)
    {
        std::vector<FenceHandle> fenceHandles;
        fenceHandles.reserve(fences.size());
        for (int i = 0; i < fences.size(); i++) fenceHandles.push_back(*fences[i]);
        auto result = ((DeviceHandle) *currentDevice).waitForFences(*(std::vector<vk::Fence>*) & fenceHandles, false, timeout);
    }

    /// @brief Make Fences Unsignalled
    /// @param fences 
    void Fence::ResetAll(const std::vector<const Fence*>& fences)
    {
        std::vector<FenceHandle> fenceHandles;
        fenceHandles.reserve(fences.size());
        for (int i = 0; i < fences.size(); i++) fenceHandles.push_back(*fences[i]);
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