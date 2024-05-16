#include <vulkan/vulkan.hpp>
#include "DescriptorPool.h"

namespace vg
{
    DescriptorPool::DescriptorPool(unsigned int maxSets, std::initializer_list<DescriptorPoolSize> sizes)
    {
        vk::DescriptorPoolCreateInfo info({}, maxSets);
        info.setPoolSizeCount(sizes.size());
        info.setPPoolSizes((vk::DescriptorPoolSize*) sizes.begin());
        m_handle = ((DeviceHandle) currentDevice).createDescriptorPool(info);
    }

    DescriptorPool::DescriptorPool() {}
    DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);

    }
    DescriptorPool::~DescriptorPool()
    {
        ((DeviceHandle) currentDevice).destroyDescriptorPool(m_handle);
    }

    DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
    {
        if (this == &other) return *this;

        std::swap(m_handle, other.m_handle);


        return *this;
    }
    DescriptorPool::operator const DescriptorPoolHandle& () const
    {
        return m_handle;
    }

    std::vector<DescriptorSet> DescriptorPool::Allocate(const std::vector<DescriptorSetLayoutHandle>& setLayouts)
    {
        auto sets = ((DeviceHandle) currentDevice).allocateDescriptorSets({ m_handle, setLayouts });
        return *(std::vector<DescriptorSet>*) & sets;
    }
}