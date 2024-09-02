#include <vulkan/vulkan.hpp>
#include "DescriptorPool.h"

namespace vg
{
    DescriptorPool::DescriptorPool(unsigned int maxSets, Span<const DescriptorPoolSize> sizes)
    {
        vk::DescriptorPoolCreateInfo info({ vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet }, maxSets);
        info.setPoolSizeCount(sizes.size());
        info.setPPoolSizes((vk::DescriptorPoolSize*) sizes.data());
        m_handle = ((DeviceHandle) *currentDevice).createDescriptorPool(info);
    }

    DescriptorPool::DescriptorPool() : m_handle(nullptr) {}
    DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
        :DescriptorPool()
    {
        *this = std::move(other);
    }
    DescriptorPool::~DescriptorPool()
    {
        if (!m_handle)return;
        ((DeviceHandle) *currentDevice).destroyDescriptorPool(m_handle);
        m_handle = nullptr;
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

    void DescriptorPool::Allocate(Span<const DescriptorSetLayoutHandle> setLayouts, std::vector<DescriptorSet>* descriptors)
    {
        auto sets = ((DeviceHandle) *currentDevice).allocateDescriptorSets({ m_handle, setLayouts });
        descriptors->resize(sets.size());
        for (int i = 0; i < sets.size(); i++)
            descriptors[0][i] = DescriptorSet(sets[i], m_handle);
    }

    void DescriptorPool::Allocate(DescriptorSetLayoutHandle setLayout, DescriptorSet* descriptors)
    {
        std::vector<DescriptorSetLayoutHandle> setLayouts = { setLayout };
        auto sets = ((DeviceHandle) *currentDevice).allocateDescriptorSets({ m_handle, setLayouts });
        for (int i = 0; i < sets.size(); i++)
            *descriptors = DescriptorSet(sets[i], m_handle);
    }
}