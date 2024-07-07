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
        MemoryBlock(DeviceMemoryHandle memory) : m_handle(memory), m_referanceCount(0) {}

        operator DeviceMemoryHandle() const;

        void Bind(Buffer* buffer, uint64_t offset);
        void Bind(Image* buffer, uint64_t offset);

    private:
        void Dereferance();
        int m_referanceCount;
        DeviceMemoryHandle m_handle;

        friend class vg::Buffer;
        friend class vg::Image;
    };
}