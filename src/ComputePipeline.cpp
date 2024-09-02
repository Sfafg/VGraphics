#include <vulkan/vulkan.hpp>
#include "Device.h"
#include "ComputePipeline.h"

namespace vg
{
    ComputePipeline::ComputePipeline(const Shader& shader, Span<const Span<const DescriptorSetLayoutBinding>> setLayoutBindings, Span<const PushConstantRange> pushConstantRanges, PipelineCacheHandle cache)
    {

        std::vector<DescriptorSetLayoutHandle> descriptorSetLayouts(setLayoutBindings.size());
        for (int i = 0; i < descriptorSetLayouts.size(); i++)
            descriptorSetLayouts[i] = (((DeviceHandle) *currentDevice).createDescriptorSetLayout({ {}, *(Span<const vk::DescriptorSetLayoutBinding>*) & setLayoutBindings[i] }));

        PipelineLayoutHandle layout = ((DeviceHandle) *currentDevice).createPipelineLayout(vk::PipelineLayoutCreateInfo({}, descriptorSetLayouts, *(Span<const vk::PushConstantRange>*) & pushConstantRanges));
        m_pipelineLayout.m_handle = layout;
        m_pipelineLayout.m_descriptorSetLayouts = descriptorSetLayouts;

        m_handle = ((DeviceHandle) *currentDevice).createComputePipeline(cache, vk::ComputePipelineCreateInfo({}, shader, layout)).value;
    }

    ComputePipeline::ComputePipeline() :m_handle(nullptr) {}

    ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept
        :ComputePipeline()
    {
        *this = std::move(other);
    }

    ComputePipeline::~ComputePipeline()
    {
        if (m_handle == nullptr) return;


        for (const auto& descriptor : m_pipelineLayout.GetDescriptorSets())
            ((DeviceHandle) *currentDevice).destroyDescriptorSetLayout(descriptor);
        ((DeviceHandle) *currentDevice).destroyPipelineLayout(m_pipelineLayout);

        ((DeviceHandle) *currentDevice).destroyPipeline(m_handle);
        m_handle = nullptr;
    }

    ComputePipeline& ComputePipeline::operator=(ComputePipeline&& other) noexcept
    {
        if (&other == this)
            return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_pipelineLayout, other.m_pipelineLayout);

        return*this;
    }
    ComputePipeline::operator const ComputePipelineHandle& () const
    {
        return m_handle;
    }

    const PipelineLayout& ComputePipeline::GetPipelineLayout() const
    {
        return m_pipelineLayout;
    }
}
