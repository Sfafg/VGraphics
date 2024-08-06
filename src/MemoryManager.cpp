#include <vulkan/vulkan.hpp>
#include "MemoryManager.h"
#include <iostream>
#include <math.h>

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
        uint64_t currentSize = 0;
        uint32_t memoryTypeBits = ~0;
        for (int i = 0; i < buffers.size(); i++)
        {
            vk::MemoryRequirements memRequirements = ((DeviceHandle) *currentDevice).getBufferMemoryRequirements(*buffers.begin()[i]);

            buffers.begin()[i]->m_offset = currentSize;

            currentSize += memRequirements.size;
            memoryTypeBits &= memRequirements.memoryTypeBits;
        }

        vk::PhysicalDeviceMemoryProperties memProperties = ((PhysicalDeviceHandle) *currentDevice).getMemoryProperties();
        MemoryBlock* block = new MemoryBlock(((DeviceHandle) *currentDevice).allocateMemory({ currentSize, FindMemoryType(memProperties,memoryTypeBits, memoryProperty) }), currentSize);

        for (int i = 0; i < buffers.size(); i++)
            block->Bind(buffers.begin()[i]);
    }

    void Allocate(std::vector<Buffer>& buffers, Flags<MemoryProperty> memoryProperty)
    {
        uint64_t currentSize = 0;
        uint32_t memoryTypeBits = ~0;
        for (int i = 0; i < buffers.size(); i++)
        {
            vk::MemoryRequirements memRequirements = ((DeviceHandle) *currentDevice).getBufferMemoryRequirements(buffers.begin()[i]);

            buffers[i].m_offset = currentSize;

            currentSize += memRequirements.size;
            memoryTypeBits &= memRequirements.memoryTypeBits;
        }

        vk::PhysicalDeviceMemoryProperties memProperties = ((PhysicalDeviceHandle) *currentDevice).getMemoryProperties();
        MemoryBlock* block = new MemoryBlock(((DeviceHandle) *currentDevice).allocateMemory({ currentSize, FindMemoryType(memProperties,memoryTypeBits, memoryProperty) }), currentSize);

        for (int i = 0; i < buffers.size(); i++)
            block->Bind(&buffers.begin()[i]);
    }


    void Allocate(std::initializer_list<Image*> images, Flags<MemoryProperty> memoryProperty)
    {
        uint64_t currentSize = 0;
        uint32_t memoryTypeBits = ~0;
        for (int i = 0; i < images.size(); i++)
        {
            vk::MemoryRequirements memRequirements = ((DeviceHandle) *currentDevice).getImageMemoryRequirements(*images.begin()[i]);

            images.begin()[i]->m_size = memRequirements.size;
            images.begin()[i]->m_offset = currentSize;

            currentSize += memRequirements.size;
            memoryTypeBits &= memRequirements.memoryTypeBits;
        }

        vk::PhysicalDeviceMemoryProperties memProperties = ((PhysicalDeviceHandle) *currentDevice).getMemoryProperties();
        MemoryBlock* block = new MemoryBlock(((DeviceHandle) *currentDevice).allocateMemory({ currentSize, FindMemoryType(memProperties,memoryTypeBits, memoryProperty) }), currentSize);

        for (int i = 0; i < images.size(); i++)
            block->Bind(images.begin()[i]);
    }
    void Allocate(std::vector<Image>& images, Flags<MemoryProperty> memoryProperty)
    {
        uint64_t currentSize = 0;
        uint32_t memoryTypeBits = ~0;
        for (int i = 0; i < images.size(); i++)
        {
            vk::MemoryRequirements memRequirements = ((DeviceHandle) *currentDevice).getImageMemoryRequirements(images.begin()[i]);

            images[i].m_size = memRequirements.size;
            images[i].m_offset = currentSize;

            currentSize += memRequirements.size;
            memoryTypeBits &= memRequirements.memoryTypeBits;
        }

        vk::PhysicalDeviceMemoryProperties memProperties = ((PhysicalDeviceHandle) *currentDevice).getMemoryProperties();
        MemoryBlock* block = new MemoryBlock(((DeviceHandle) *currentDevice).allocateMemory({ currentSize, FindMemoryType(memProperties,memoryTypeBits, memoryProperty) }), currentSize);

        for (int i = 0; i < images.size(); i++)
            block->Bind(&images.begin()[i]);
    }

    MemoryBlock::operator DeviceMemoryHandle() const
    {
        return m_handle;
    }

    void MemoryBlock::Bind(Buffer* buffer)
    {
        m_referanceCount++;
        ((DeviceHandle) *currentDevice).bindBufferMemory((vk::Buffer) *buffer, m_handle, buffer->GetOffset());
        buffer->m_memory = this;
    }

    void MemoryBlock::Bind(Image* image)
    {
        m_referanceCount++;
        ((DeviceHandle) *currentDevice).bindImageMemory((vk::Image) *image, m_handle, image->GetOffset());
        image->m_memory = this;
    }

    void MemoryBlock::Dereferance()
    {
        m_referanceCount--;
        if (m_referanceCount <= 0)
        {
            if (m_mappedMemory != nullptr)
                UnmapMemory();

            ((DeviceHandle) *currentDevice).freeMemory(m_handle);
            delete this;
        }
    }

    char* MemoryBlock::GetMappedMemory()
    {
        if (m_mappedMemory != nullptr)
            return (char*) m_mappedMemory;

        auto result = ((DeviceHandle) *currentDevice).mapMemory(m_handle, 0, m_totalSize, {}, &m_mappedMemory);
        return (char*) m_mappedMemory;
    }

    void MemoryBlock::UnmapMemory()
    {
        if (m_mappedMemory == nullptr) return;

        ((DeviceHandle) *currentDevice).unmapMemory(m_handle);
        m_mappedMemory = nullptr;
    }
}