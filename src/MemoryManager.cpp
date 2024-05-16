#include <vulkan/vulkan.hpp>
#include "MemoryManager.h"
#include <iostream>

uint32_t FindMemoryType(vk::PhysicalDeviceMemoryProperties memProperties, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && ((uint32_t) memProperties.memoryTypes[i].propertyFlags & (uint32_t) properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
namespace vg
{
    void Allocate(std::initializer_list<Buffer*> buffers, Flags<MemoryProperty> memoryProperty)
    {
        uint64_t totalSize = 0;
        uint32_t memoryTypeBits = ~0;
        for (int i = 0; i < buffers.size(); i++)
        {
            vk::MemoryRequirements memRequirements = ((DeviceHandle) currentDevice).getBufferMemoryRequirements(*buffers.begin()[i]);
            totalSize += memRequirements.size;
            memoryTypeBits &= memRequirements.memoryTypeBits;
        }

        vk::PhysicalDeviceMemoryProperties memProperties = ((PhysicalDeviceHandle) currentDevice).getMemoryProperties();
        MemoryBlock* block = new MemoryBlock(((DeviceHandle) currentDevice).allocateMemory({ totalSize, FindMemoryType(memProperties,memoryTypeBits, memoryProperty) }));

        uint64_t offset = 0;
        for (int i = 0; i < buffers.size(); i++)
        {
            block->Bind(buffers.begin()[i], offset);
            offset += buffers.begin()[i]->GetSize();
        }
    }

    void Allocate(std::vector<Buffer>& buffers, Flags<MemoryProperty> memoryProperty)
    {
        uint64_t totalSize = 0;
        uint32_t memoryTypeBits = ~0;
        for (int i = 0; i < buffers.size(); i++)
        {
            vk::MemoryRequirements memRequirements = ((DeviceHandle) currentDevice).getBufferMemoryRequirements(buffers.begin()[i]);
            totalSize += memRequirements.size;
            memoryTypeBits &= memRequirements.memoryTypeBits;
        }

        vk::PhysicalDeviceMemoryProperties memProperties = ((PhysicalDeviceHandle) currentDevice).getMemoryProperties();
        MemoryBlock* block = new MemoryBlock(((DeviceHandle) currentDevice).allocateMemory({ totalSize, FindMemoryType(memProperties,memoryTypeBits, memoryProperty) }));

        uint64_t offset = 0;
        for (int i = 0; i < buffers.size(); i++)
        {
            block->Bind(&buffers.begin()[i], offset);
            offset += buffers.begin()[i].GetSize();
        }
    }

    MemoryBlock::operator DeviceMemoryHandle() const
    {
        return m_handle;
    }

    void MemoryBlock::Bind(Buffer* buffer, uint64_t offset)
    {
        m_referanceCount++;
        ((DeviceHandle) currentDevice).bindBufferMemory((vk::Buffer) *buffer, m_handle, offset);
        buffer->m_memory = this;
        buffer->m_offset = offset;
    }

    void MemoryBlock::Dereferance()
    {
        m_referanceCount--;
        if (m_referanceCount <= 0)
        {
            ((DeviceHandle) currentDevice).freeMemory(m_handle);
            delete this;
        }
    }
}