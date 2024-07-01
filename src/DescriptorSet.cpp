#include <vulkan/vulkan.hpp>
#include "DescriptorSet.h"

namespace vg
{
    DescriptorSet::operator const DescriptorSetHandle& () const
    {
        return m_handle;
    }
    void DescriptorSet::AttachBuffer(const Buffer& buffer, int offset, int range, int binding, int arrayElement)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = (BufferHandle) buffer;
        bufferInfo.offset = offset;
        bufferInfo.range = range;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_handle;
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = arrayElement;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        ((vk::Device) currentDevice).updateDescriptorSets({ descriptorWrite }, {});
    }
}