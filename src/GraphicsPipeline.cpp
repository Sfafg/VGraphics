#include <vulkan/vulkan.hpp>
#include "GraphicsPipeline.h"

namespace vg
{
    GraphicsPipeline::GraphicsPipeline(
        const std::vector<Shader*>& shaders,
        const VertexLayout& vertexInput,
        const InputAssembly& inputAssembly,
        const Tesselation& tesselation,
        const ViewportState& viewportState,
        const Rasterizer& rasterizer,
        const Multisampling& multisampling,
        const DepthStencil& depthStencil,
        const ColorBlending& colorBlending,
        const std::vector<DynamicState>& dynamicState,
        const std::vector<DescriptorSetLayoutBinding>& setLayoutBindings)
        :shaders(shaders), vertexInput(vertexInput), inputAssembly(inputAssembly), tesselation(tesselation), viewportState(viewportState), rasterizer(rasterizer), multisampling(multisampling), depthStencil(depthStencil), colorBlending(colorBlending), dynamicState(dynamicState), setLayoutBindings(setLayoutBindings)
    {}
}