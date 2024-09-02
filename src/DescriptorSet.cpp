#include <vulkan/vulkan.hpp>
#include "DescriptorSet.h"

namespace vg
{
    DescriptorSet::DescriptorSet() :m_handle(nullptr), m_pool(nullptr) {}

    DescriptorSet::DescriptorSet(DescriptorSetHandle handle, DescriptorPoolHandle pool) :m_handle(handle), m_pool(pool) {}

    DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
        :DescriptorSet()
    {
        *this = std::move(other);
    }

    DescriptorSet::~DescriptorSet()
    {
        if (!m_handle) return;
        ((DeviceHandle) *currentDevice).freeDescriptorSets(m_pool, { m_handle });
        m_handle = nullptr;
    }

    DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
    {
        if (&other == this)return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_pool, other.m_pool);

        return *this;
    }

    DescriptorSet::operator const DescriptorSetHandle& () const
    {
        return m_handle;
    }

    void DescriptorSet::AttachBuffer(DescriptorType descriptorType, const Buffer& buffer, int offset, int range, int binding, int arrayElement)
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
        descriptorWrite.descriptorType = (VkDescriptorType) descriptorType;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        ((DeviceHandle) *currentDevice).updateDescriptorSets({ descriptorWrite }, {});
    }

    void DescriptorSet::AttachImage(DescriptorType descriptorType, ImageLayout layout, const ImageView& imageView, const Sampler& sampler, int binding, int arrayElement)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = (VkImageLayout) layout;
        imageInfo.imageView = (ImageViewHandle) imageView;
        imageInfo.sampler = (SamplerHandle) sampler;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_handle;
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = arrayElement;
        descriptorWrite.descriptorType = (VkDescriptorType) descriptorType;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;
        ((DeviceHandle) *currentDevice).updateDescriptorSets({ descriptorWrite }, {});
    }
}