#pragma once
#include "Buffer.h"
#include "Image.h"
#include "Enums.h"
#include "Flags.h"
#include "Device.h"

namespace vg
{
    extern void Allocate(std::initializer_list<Buffer*> buffers, Flags<MemoryProperty> memoryProperty);
    extern void Allocate(std::vector<Buffer>& buffers, Flags<MemoryProperty> memoryProperty);
    inline void Allocate(Buffer* buffer, Flags<MemoryProperty> memoryProperty)
    {
        Allocate({ buffer }, memoryProperty);
    }

    extern void Allocate(std::initializer_list<Image*> images, Flags<MemoryProperty> memoryProperty);
    extern void Allocate(std::vector<Image>& images, Flags<MemoryProperty> memoryProperty);
    inline void Allocate(Image* image, Flags<MemoryProperty> memoryProperty)
    {
        Allocate({ image }, memoryProperty);
    }

    class MemoryBlock
    {
    public:
        MemoryBlock(DeviceMemoryHandle memory, uint64_t totalSize) : m_handle(memory), m_referanceCount(0), m_totalSize(totalSize), m_mappedMemory(nullptr) {}

        operator DeviceMemoryHandle() const;

        void Bind(Buffer* buffer);
        void Bind(Image* buffer);

    private:
        void Dereferance();
        void* GetMappedMemory();
        void UnmapMemory();
        int m_referanceCount;
        DeviceMemoryHandle m_handle;
        uint64_t m_totalSize;
        void* m_mappedMemory;

        friend class vg::Buffer;
        friend class vg::Image;
    };
}