#pragma once
#include "Buffer.h"
#include "Enums.h"
#include "Flags.h"
#include "Device.h"

namespace vg
{
    extern void Allocate(const Device& device, std::initializer_list<Buffer*> buffers, Flags<MemoryProperty> memoryProperty);
    extern void Allocate(const Device& device, std::vector<Buffer>& buffers, Flags<MemoryProperty> memoryProperty);
    inline void Allocate(const Device& device, Buffer* buffer, Flags<MemoryProperty> memoryProperty)
    {
        Allocate(device, { buffer }, memoryProperty);
    }

    class MemoryBlock
    {
    public:
        MemoryBlock(DeviceHandle device, DeviceMemoryHandle memory) :m_device(device), m_handle(memory), m_referanceCount(0) {}

        operator DeviceMemoryHandle() const;

        void Bind(Buffer* buffer, uint64_t offset);

    private:
        void Dereferance();
        int m_referanceCount;
        DeviceMemoryHandle m_handle;
        DeviceHandle m_device;
        friend class vg::Buffer;
    };
}