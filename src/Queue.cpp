#include <vulkan/vulkan.hpp>
#include "Queue.h"
#include "Synchronization.h"

namespace vg
{
    Queue::Queue() :m_handle(nullptr), m_commandPool(nullptr), m_type(Type::None), m_index(0), m_device(nullptr) {}

    Queue::Queue(Queue&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_commandPool, other.m_commandPool);
        std::swap(m_type, other.m_type);
        std::swap(m_index, other.m_index);
        std::swap(m_device, other.m_device);
        other.m_handle = nullptr;
    }
    Queue::~Queue()
    {
        if (m_commandPool == nullptr || m_device == nullptr)return;
        m_device.destroyCommandPool(m_commandPool);
        m_commandPool = nullptr;
    }
    Queue& Queue::operator=(Queue&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_commandPool, other.m_commandPool);
        std::swap(m_type, other.m_type);
        std::swap(m_index, other.m_index);
        std::swap(m_device, other.m_device);

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

    DeviceHandle Queue::GetDevice() const
    {
        return m_device;
    }

    void Queue::Submit(const std::vector<SubmitInfo>& submits, const Fence& fence)
    {
        vk::SubmitInfo* submits_ = new vk::SubmitInfo[submits.size()];
        vk::Semaphore** waitSemaphores = new vk::Semaphore * [submits.size()];
        vk::PipelineStageFlags** stages = new vk::PipelineStageFlags * [submits.size()];
        for (unsigned int i = 0; i < (unsigned int) submits.size(); i++)
        {
            waitSemaphores[i] = new vk::Semaphore[submits[i].waitStages.size()];
            stages[i] = new vk::PipelineStageFlags[submits[i].waitStages.size()];
            for (unsigned int j = 0; j < (unsigned int) submits[i].waitStages.size(); j++)
            {
                waitSemaphores[i][j] = (vk::Semaphore) submits[i].waitStages[j].semaphore;
                stages[i][j] = (vk::PipelineStageFlagBits) submits[i].waitStages[j].stage;
            }
            vk::SubmitInfo sub(submits[i].waitStages.size(), waitSemaphores[i], stages[i], submits[i].commanBuffers.size(), (vk::CommandBuffer*) submits[i].commanBuffers.data(), submits[i].signalSemaphores.size(), (vk::Semaphore*) submits[i].signalSemaphores.data());
            submits_[i] = sub;
        }

        auto result = m_handle.submit(submits.size(), submits_, (FenceHandle) fence);
        delete[] submits_;
        delete[] waitSemaphores;
        delete[] stages;
    }
    void Queue::Present(const std::vector<SemaphoreHandle>& waitSemaphores, const std::vector<SwapchainHandle>& swapchains, const std::vector<uint32_t>& imageIndices)
    {
        auto result = m_handle.presentKHR({ *(std::vector<vk::Semaphore>*) & waitSemaphores,*(std::vector<vk::SwapchainKHR>*) & swapchains,imageIndices });
    }
}