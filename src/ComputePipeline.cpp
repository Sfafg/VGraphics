#include <vulkan/vulkan.hpp>
#include "Device.h"
#include "ComputePipeline.h"

namespace vg {
ComputePipeline::ComputePipeline(const Shader &shader, PipelineLayout &&layout, PipelineCacheHandle cache) {
    m_pipelineLayout = std::move(layout);
    m_handle = ((DeviceHandle)*currentDevice)
                   .createComputePipeline(cache, vk::ComputePipelineCreateInfo({}, shader, m_pipelineLayout.m_handle))
                   .value;
}

ComputePipeline::ComputePipeline() : m_handle(nullptr) {}

ComputePipeline::ComputePipeline(ComputePipeline &&other) noexcept : ComputePipeline() { *this = std::move(other); }

ComputePipeline::~ComputePipeline() {
    if (m_handle == nullptr) return;

    ((DeviceHandle)*currentDevice).destroyPipeline(m_handle);
    m_handle = nullptr;
}

ComputePipeline &ComputePipeline::operator=(ComputePipeline &&other) noexcept {
    if (&other == this) return *this;

    std::swap(m_handle, other.m_handle);
    std::swap(m_pipelineLayout, other.m_pipelineLayout);

    return *this;
}
ComputePipeline::operator const ComputePipelineHandle &() const { return m_handle; }

const PipelineLayout &ComputePipeline::GetPipelineLayout() const { return m_pipelineLayout; }
} // namespace vg
