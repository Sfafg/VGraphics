#include <vulkan/vulkan.hpp>
#include "GraphicsPipeline.h"
#include "Device.h"
#include "Handle.h"
#include <iostream>

namespace vg {
GraphicsPipeline::GraphicsPipeline(
    uint32_t pipelineLayout, const std::vector<const Shader *> &shaders, const VertexLayout &vertexInput,
    const InputAssembly &inputAssembly, const Tesselation &tesselation, const ViewportState &viewportState,
    const Rasterizer &rasterizer, const Multisampling &multisampling, const DepthStencil &depthStencil,
    const ColorBlending &colorBlending, const std::vector<DynamicState> &dynamicState, int parentIndex,
    GraphicsPipelineHandle parent
)
    : pipelineLayout(pipelineLayout), shaders(shaders), vertexInput(vertexInput), inputAssembly(inputAssembly),
      tesselation(tesselation), viewportState(viewportState), rasterizer(rasterizer), multisampling(multisampling),
      depthStencil(depthStencil), colorBlending(colorBlending), dynamicState(dynamicState), parentIndex(parentIndex),
      parent(parent) {}

GraphicsPipeline::GraphicsPipeline() {}
} // namespace vg
