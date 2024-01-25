#pragma once
#include <vector>
#include "Handle.h"
#include "Enums.h"

namespace vg
{
    class Device;
    class Fence;
    /**
     *@brief Holds information about awaiting stage
     *
     */
    struct WaitStage
    {
        PipelineStage stage;
        SemaphoreHandle semaphore;
        /**
         *@brief Construct a new Wait Stage object
         *
         * @param stage Stage at which to wait
         * @param semaphore Semaphore for which to wait
         */
        WaitStage(PipelineStage stage, SemaphoreHandle semaphore) :stage(stage), semaphore(semaphore) {}
    };
    /**
     *@brief Holds information about submiting command buffer
     *
     */
    struct SubmitInfo
    {
        std::vector<WaitStage> waitStages;
        std::vector<CommandBufferHandle> commandBuffers;
        std::vector<SemaphoreHandle> signalSemaphores;
        /**
         * @brief Construct a new Submit Info object
         *
         * @param waitStages Array of wait stages
         * @param commandBuffers Array of command buffers to submit
         * @param signalSemaphores Semaphores signalled upon all command buffers complete execution
         */
        SubmitInfo(const std::vector<WaitStage>& waitStages, const std::vector<CommandBufferHandle>& commandBuffers, const std::vector<SemaphoreHandle>& signalSemaphores)
            :waitStages(waitStages), commandBuffers(commandBuffers), signalSemaphores(signalSemaphores)
        {}
    };

    /**
     *@brief Device Queue
     * Devices have different Queues with differing capabilities, this class is meant to handle creation of Queues.
     */
    class Queue
    {
    public:
        /**
         *@brief Type of Queue
         *
         */
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

        /**
         *@brief Get the Type object
         *
         * @return Type
         */
        Type GetType() const;
        /**
         *@brief Get the Index object
         *
         * @return unsigned int
         */
        unsigned int GetIndex() const;
        /**
         *@brief Get the Command Pool object
         *
         * @return CommandPoolHandle
         */
        CommandPoolHandle GetCommandPool() const;
        /**
         *@brief Get the Device object
         *
         * @return DeviceHandle
         */
        DeviceHandle GetDevice() const;
        /**
         *@brief Submit command buffers and all relevant data
         *
         * @param submits Command buffers and relevant data
         * @param fence Fence to be signaled upon all submits finish
         */
        void Submit(const std::vector<SubmitInfo>& submits, const Fence& fence);
        /**
         *@brief Present image to the window. Queue Type must be Queue::Present
         *
         * @param waitSemaphores Semaphores to await before presenting
         * @param swapchains array of Swapchains used for rendering
         * @param imageIndices indices of images from Swapchains
         *
         */
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