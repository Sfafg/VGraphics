#pragma once
#include <vector>
#include "Handle.h"
#include "Enums.h"
#include "Flags.h"
namespace vg
{
    /**
     *@brief ((DeviceHandle)currentDevice).Queue
     * ((DeviceHandle)currentDevice). have different Queues with differing capabilities, this class is meant to handle creation of Queues.
     */
    class Queue
    {
    public:
        Queue();
        Queue(Queue&& other) noexcept;
        Queue(const Queue& other) = delete;
        ~Queue();

        Queue& operator=(Queue&& other) noexcept;
        Queue& operator=(const Queue& other) = delete;
        operator const QueueHandle& () const;

        /**
         *@brief Get the QueueType object
         *
         * @return QueueType
         */
        QueueType GetType() const;
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
        QueueType m_type;
        unsigned int m_index;


        friend class Device;
    };
}