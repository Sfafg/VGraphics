#include <iostream>
#include <vulkan/vulkan.hpp>
#include "PipelineLayout.h"
#include "Device.h"

namespace vg {
PipelineLayout::PipelineLayout() {}

PipelineLayout::PipelineLayout(
    const std::vector<std::vector<DescriptorSetLayoutBinding>> &setLayoutBindings,
    const std::vector<PushConstantRange> &pushConstantRanges
) {
    std::vector<DescriptorSetLayoutHandle> descriptorSetLayouts(setLayoutBindings.size());
    for (int i = 0; i < descriptorSetLayouts.size(); i++)
        descriptorSetLayouts[i] =
            (((DeviceHandle)*currentDevice)
                 .createDescriptorSetLayout({{}, *(std::vector<vk::DescriptorSetLayoutBinding> *)&setLayoutBindings[i]})
            );

    PipelineLayoutHandle layout =
        ((DeviceHandle)*currentDevice)
            .createPipelineLayout(vk::PipelineLayoutCreateInfo(
                {}, descriptorSetLayouts, *(std::vector<vk::PushConstantRange> *)&pushConstantRanges
            ));
    m_handle = layout;
    m_descriptorSetLayouts = descriptorSetLayouts;
}
PipelineLayout::PipelineLayout(PipelineLayout &&other) noexcept : PipelineLayout() { *this = std::move(other); }

PipelineLayout &PipelineLayout::operator=(PipelineLayout &&other) noexcept {
    if (&other == this) return *this;

    std::swap(m_handle, other.m_handle);
    std::swap(m_descriptorSetLayouts, other.m_descriptorSetLayouts);

    return *this;
}
PipelineLayout::~PipelineLayout() {
    if (!m_handle) return;

    for (int i = 0; i < m_descriptorSetLayouts.size(); i++)
        ((DeviceHandle)*currentDevice).destroyDescriptorSetLayout(m_descriptorSetLayouts[i]);
    ((DeviceHandle)*currentDevice).destroyPipelineLayout(m_handle);
    m_handle = nullptr;
}
PipelineLayout::operator const PipelineLayoutHandle &() const { return m_handle; }
Span<DescriptorSetLayoutHandle> PipelineLayout::GetDescriptorSets() { return m_descriptorSetLayouts; }
Span<const DescriptorSetLayoutHandle> PipelineLayout::GetDescriptorSets() const { return m_descriptorSetLayouts; }
} // namespace vg
