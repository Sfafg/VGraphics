#include <vulkan/vulkan.hpp>
#include "PipelineLayout.h"

namespace vg
{
    PipelineLayout::PipelineLayout()
    {}

    PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
        :PipelineLayout()
    {
        *this = std::move(other);
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
    Span<DescriptorSetLayoutHandle> PipelineLayout::GetDescriptorSets()
    {
        return m_descriptorSetLayouts;
    }
    Span<const DescriptorSetLayoutHandle> PipelineLayout::GetDescriptorSets() const
    {
        return m_descriptorSetLayouts;
    }
}