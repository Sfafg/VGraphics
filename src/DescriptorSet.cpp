#include <vulkan/vulkan.hpp>
#include "DescriptorSet.h"

namespace vg
{
    DescriptorSet::operator const DescriptorSetHandle& () const
    {
        return m_handle;
    }
    void DescriptorSet::SetBufferInfo(Buffer buffer, uint64_t offset, uint64_t range, unsigned int dstBinding, unsigned int dstArrayElement, DescriptorType type)
    {
        // vk::DescriptorBufferInfo bufferInfo(buffer, offset, range);
        // vk::WriteDescriptorSet descriptorWrite(m_handle, dstBinding, dstArrayElement, (vk::DescriptorType) type, 1, &bufferInfo);

        // ((((DeviceHandle)currentDevice).andle) ((DeviceHandle)currentDevice)..updateDescriptorSets()
    }
}