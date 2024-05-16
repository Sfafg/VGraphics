#include <vulkan/vulkan.hpp>
#include "DescriptorPool.h"

namespace vg
{
    DescriptorPool::DescriptorPool(const Device& device, unsigned int maxSets, std::initializer_list<DescriptorPoolSize> sizes) : m_device(device)
    {
        vk::DescriptorPoolCreateInfo info({}, maxSets);
        info.setPoolSizeCount(sizes.size());
        info.setPPoolSizes((vk::DescriptorPoolSize*) sizes.begin());
        m_handle = m_device.createDescriptorPool(info);
    }

    DescriptorPool::DescriptorPool() {}
    DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);
    }
    DescriptorPool::~DescriptorPool()
    {
        m_device.destroyDescriptorPool(m_handle);
    }

    DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
    {
        if (this == &other) return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);

        return *this;
    }
    DescriptorPool::operator const DescriptorPoolHandle& () const
    {
        return m_handle;
    }

    std::vector<DescriptorSet> DescriptorPool::Allocate(const std::vector<DescriptorSetLayoutHandle>& setLayouts)
    {
        auto sets = m_device.allocateDescriptorSets({ m_handle, setLayouts });
        return *(std::vector<DescriptorSet>*) & sets;
    }
}