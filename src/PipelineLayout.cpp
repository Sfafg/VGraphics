#include <vulkan/vulkan.hpp>
#include "PipelineLayout.h"

namespace vg
{
    PipelineLayout::PipelineLayout()
    {}

    PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_descriptorSetLayouts, other.m_descriptorSetLayouts);
    }

    PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other) noexcept
    {
        if (&other == this) return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_descriptorSetLayouts, other.m_descriptorSetLayouts);

        return *this;
    }
    PipelineLayout::operator const PipelineLayoutHandle& () const
    {
        return m_handle;
    }
    std::vector<DescriptorSetLayoutHandle>& PipelineLayout::GetDescriptorSets()
    {
        return m_descriptorSetLayouts;
    }
    const std::vector<DescriptorSetLayoutHandle>& PipelineLayout::GetDescriptorSets() const
    {
        return m_descriptorSetLayouts;
    }
}