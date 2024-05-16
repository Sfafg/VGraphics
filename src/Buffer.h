#pragma once
#include "Handle.h"
#include "Flags.h"
#include "Enums.h"

namespace vg
{
    class Buffer
    {
    public:
        Buffer();
        Buffer(uint64_t byteSize, Flags<BufferUsage> usage, BufferSharing sharing = BufferSharing::Exclusive);
        Buffer(Buffer&& other) noexcept;
        Buffer(const Buffer& other) = delete;
        ~Buffer();

        Buffer& operator=(Buffer&& other) noexcept;
        Buffer& operator=(const Buffer& other) = delete;
        operator const BufferHandle& () const;

        uint64_t GetSize() const;
        uint64_t GetOffset() const;
        class MemoryBlock* GetMemory() const;

        void* MapMemory();
        void UnmapMemory();

    private:
        BufferHandle m_handle;

        uint64_t m_offset;
        uint64_t m_size;
        class MemoryBlock* m_memory;

        friend class MemoryBlock;
    };
}