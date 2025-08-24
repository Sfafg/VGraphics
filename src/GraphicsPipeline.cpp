#include <vulkan/vulkan.hpp>
#include "GraphicsPipeline.h"
#include "Device.h"
#include <iostream>

namespace vg {
GraphicsPipeline::GraphicsPipeline(
    uint32_t pipelineLayout, const std::vector<Shader *> &shaders, const VertexLayout &vertexInput,
    const InputAssembly &inputAssembly, const Tesselation &tesselation, const ViewportState &viewportState,
    const Rasterizer &rasterizer, const Multisampling &multisampling, const DepthStencil &depthStencil,
    const ColorBlending &colorBlending, const std::vector<DynamicState> &dynamicState, int parentIndex,
    GraphicsPipeline *parent
)
    : m_handle(nullptr), pipelineLayout(pipelineLayout), shaders(shaders), vertexInput(vertexInput),
      inputAssembly(inputAssembly), tesselation(tesselation), viewportState(viewportState), rasterizer(rasterizer),
      multisampling(multisampling), depthStencil(depthStencil), colorBlending(colorBlending),
      dynamicState(dynamicState), parentIndex(parentIndex),
      parent(parent ? parent->m_handle : GraphicsPipelineHandle()) {}

GraphicsPipeline::GraphicsPipeline(
    uint32_t pipelineLayout, std::vector<Shader> &&shaders, const VertexLayout &vertexInput,
    const InputAssembly &inputAssembly, const Tesselation &tesselation, const ViewportState &viewportState,
    const Rasterizer &rasterizer, const Multisampling &multisampling, const DepthStencil &depthStencil,
    const ColorBlending &colorBlending, const std::vector<DynamicState> &dynamicState, int parentIndex,
    GraphicsPipeline *parent
)
    : m_handle(nullptr), pipelineLayout(pipelineLayout), shaders_(std::move(shaders)), vertexInput(vertexInput),
      inputAssembly(inputAssembly), tesselation(tesselation), viewportState(viewportState), rasterizer(rasterizer),
      multisampling(multisampling), depthStencil(depthStencil), colorBlending(colorBlending),
      dynamicState(dynamicState), parentIndex(parentIndex),
      parent(parent ? parent->m_handle : GraphicsPipelineHandle()) {
    this->shaders = std::vector<Shader *>(shaders_.size());
    for (int i = 0; i < shaders_.size(); i++) this->shaders[i] = &shaders_[i];
}

GraphicsPipeline::GraphicsPipeline() : m_handle(nullptr) {}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&other) noexcept : GraphicsPipeline() { *this = std::move(other); }

GraphicsPipeline &GraphicsPipeline::operator=(GraphicsPipeline &&other) noexcept {
    if (&other == this) return *this;

    std::swap(m_handle, other.m_handle);
    std::swap(pipelineLayout, other.pipelineLayout);
    std::swap(shaders, other.shaders);
    std::swap(shaders_, other.shaders_);
    std::swap(vertexInput, other.vertexInput);
    std::swap(inputAssembly, other.inputAssembly);
    std::swap(tesselation, other.tesselation);
    std::swap(viewportState, other.viewportState);
    std::swap(rasterizer, other.rasterizer);
    std::swap(multisampling, other.multisampling);
    std::swap(depthStencil, other.depthStencil);
    std::swap(colorBlending, other.colorBlending);
    std::swap(dynamicState, other.dynamicState);
    std::swap(parentIndex, other.parentIndex);
    std::swap(parent, other.parent);

    return *this;
}

GraphicsPipeline::operator const GraphicsPipelineHandle &() const { return m_handle; }

} // namespace vg
