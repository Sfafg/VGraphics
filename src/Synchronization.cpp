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
        :Fence()
    {
        *this = std::move(other);
    }

    Fence::~Fence()
    {
        if (m_handle == nullptr) return;
        ((DeviceHandle) *currentDevice).destroyFence(m_handle);
        m_handle = nullptr;
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
        Fence::AwaitAll({ this }, reset, timeout);
    }

    /// @brief  Make Fence Unsignalled
    void Fence::Reset()
    {
        Fence::ResetAll({ this });
    }

    /// @brief Await untill all fence enter the signalled state.
    /// @param fences 
    /// @param timeout 
    void Fence::AwaitAll(std::span<Fence> fences, bool reset, uint64_t timeout)
    {
        auto result = ((DeviceHandle) *currentDevice).waitForFences(*(std::span<const vk::Fence>*) & fences, true, timeout);
        if (reset)
            Fence::ResetAll(fences);
    }

    void Fence::AwaitAll(Span<Fence* const> fences, bool reset, uint64_t timeout)
    {
        std::vector<vk::Fence> fences_(fences.size());
        for (int i = 0; i < fences.size(); i++)fences_[i] = fences[i]->m_handle;
        auto result = ((DeviceHandle) *currentDevice).waitForFences(fences_, true, timeout);
        if (reset)
            Fence::ResetAll(fences);
    }


    /// @brief Await untill any fence enter the signalled state.
    /// @param fences 
    /// @param timeout 
    void Fence::AwaitAny(std::span<Fence> fences, uint64_t timeout)
    {
        auto result = ((DeviceHandle) *currentDevice).waitForFences(*(std::span<const vk::Fence>*) & fences, false, timeout);
    }

    void Fence::AwaitAny(Span<Fence* const> fences, uint64_t timeout)
    {
        std::vector<vk::Fence> fences_(fences.size());
        for (int i = 0; i < fences.size(); i++)fences_[i] = fences[i]->m_handle;
        auto result = ((DeviceHandle) *currentDevice).waitForFences(fences_, false, timeout);
    }

    /// @brief Make Fences Unsignalled
    /// @param fences 
    void Fence::ResetAll(std::span<Fence> fences)
    {
        ((DeviceHandle) *currentDevice).resetFences(*(std::span<const vk::Fence>*) & fences);
    }

    void Fence::ResetAll(Span<Fence* const> fences)
    {
        std::vector<vk::Fence> fences_(fences.size());
        for (int i = 0; i < fences.size(); i++)fences_[i] = fences[i]->m_handle;
        ((DeviceHandle) *currentDevice).resetFences(fences_);
    }

    Semaphore::Semaphore()
    {
        m_handle = ((DeviceHandle) *currentDevice).createSemaphore({});
    }

    Semaphore::Semaphore(void* ptr) : m_handle(nullptr) { assert(ptr == nullptr); }
    Semaphore::Semaphore(Semaphore&& other) noexcept
        :Semaphore()
    {
        *this = std::move(other);
    }

    Semaphore::~Semaphore()
    {
        if (m_handle == nullptr) return;
        vkDestroySemaphore((DeviceHandle) *currentDevice, m_handle, nullptr);
        m_handle = nullptr;
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