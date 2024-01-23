#pragma once
#include <vector>
#include "Handle.h"
#include "Enums.h"

namespace vg
{
    class Device;
    class Fence;
    struct WaitStage
    {
        PipelineStage stage;
        SemaphoreHandle semaphore;
        WaitStage(PipelineStage stage, SemaphoreHandle semaphore) :stage(stage), semaphore(semaphore) {}
    };
    struct SubmitInfo
    {
        std::vector<WaitStage> waitStages;
        std::vector<CommandBufferHandle> commanBuffers;
        std::vector<SemaphoreHandle> signalSemaphores;
        SubmitInfo(const std::vector<WaitStage>& waitStages, const std::vector<CommandBufferHandle>& commanBuffers, const std::vector<SemaphoreHandle>& signalSemaphores)
            :waitStages(waitStages), commanBuffers(commanBuffers), signalSemaphores(signalSemaphores)
        {}
    };

    class Queue
    {
    public:
        enum class Type
        {
            None = 0,
            Graphics,
            Compute,
            Present,
            Transfer
        };

    public:
        Queue();
        Queue(Queue&& other) noexcept;
        Queue(const Queue& other) = delete;
        ~Queue();

        Queue& operator=(Queue&& other) noexcept;
        Queue& operator=(const Queue& other) = delete;
        operator const QueueHandle& () const;

        Type GetType() const;
        unsigned int GetIndex() const;
        CommandPoolHandle GetCommandPool() const;
        DeviceHandle GetDevice() const;
        void Submit(const std::vector<SubmitInfo>& submits, const Fence& fence);
        void Present(const std::vector<SemaphoreHandle>& waitSemaphores, const std::vector<SwapchainHandle>& swapchains, const std::vector<uint32_t>& imageIndices);

    private:
        QueueHandle m_handle;
        CommandPoolHandle m_commandPool;
        Type m_type;
        unsigned int m_index;
        DeviceHandle m_device;

        friend Device;
    };
}