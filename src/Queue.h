#pragma once
#include "Handle.h"

namespace vg
{
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
        Queue(QueueHandle handle, Type type, unsigned int index);

        Queue(Queue&& other) noexcept;
        Queue& operator=(Queue&& other) noexcept;
        Queue(const Queue& other) = delete;
        Queue& operator=(const Queue& other) = delete;

        Type GetType() const;
        unsigned int GetIndex() const;
        operator QueueHandle() const;

    private:
        QueueHandle m_handle;
        Type m_type;
        unsigned int m_index;
    };
}