#include <vulkan/vulkan.hpp>
#include "GraphicsPipeline.h"

namespace vg
{
    GraphicsPipeline::GraphicsPipeline(
        const std::vector<DescriptorSetLayoutBinding>& setLayoutBindings,
        const std::vector<Shader*>& shaders,
        const VertexLayout& vertexInput,
        const InputAssembly& inputAssembly,
        const Tesselation& tesselation,
        const ViewportState& viewportState,
        const Rasterizer& rasterizer,
        const Multisampling& multisampling,
        const DepthStencil& depthStencil,
        const ColorBlending& colorBlending,
        const std::vector<DynamicState>& dynamicState)
        :
        shaders(shaders),
        vertexInput(vertexInput),
        inputAssembly(inputAssembly),
        tesselation(tesselation),
        viewportState(viewportState),
        rasterizer(rasterizer),
        multisampling(multisampling),
        depthStencil(depthStencil),
        colorBlending(colorBlending),
        dynamicState(dynamicState),
        setLayoutBindings(setLayoutBindings),
        parentPipelineIndex(-1),
        parentPipeline(nullptr)
    {}

    GraphicsPipeline::GraphicsPipeline(
        int parentIndex,
        const std::vector<DescriptorSetLayoutBinding>& setLayoutBindings,
        const std::optional<std::vector<Shader*>>& shaders,
        const std::optional<VertexLayout>& vertexInput,
        const std::optional<InputAssembly>& inputAssembly,
        const std::optional<Tesselation>& tesselation,
        const std::optional<ViewportState>& viewportState,
        const std::optional<Rasterizer>& rasterizer,
        const std::optional<Multisampling>& multisampling,
        const std::optional<DepthStencil>& depthStencil,
        const std::optional<ColorBlending>& colorBlending,
        const std::optional<std::vector<DynamicState>>& dynamicState)
        :
        shaders(shaders),
        vertexInput(vertexInput),
        inputAssembly(inputAssembly),
        tesselation(tesselation),
        viewportState(viewportState),
        rasterizer(rasterizer),
        multisampling(multisampling),
        depthStencil(depthStencil),
        colorBlending(colorBlending),
        dynamicState(dynamicState),
        setLayoutBindings(setLayoutBindings),
        parentPipelineIndex(parentIndex),
        parentPipeline(nullptr)
    {}

    GraphicsPipeline::GraphicsPipeline(
        GraphicsPipelineHandle parentPipeline,
        const std::vector<DescriptorSetLayoutBinding>& setLayoutBindings,
        const std::optional<std::vector<Shader*>>& shaders,
        const std::optional<VertexLayout>& vertexInput,
        const std::optional<InputAssembly>& inputAssembly,
        const std::optional<Tesselation>& tesselation,
        const std::optional<ViewportState>& viewportState,
        const std::optional<Rasterizer>& rasterizer,
        const std::optional<Multisampling>& multisampling,
        const std::optional<DepthStencil>& depthStencil,
        const std::optional<ColorBlending>& colorBlending,
        const std::optional<std::vector<DynamicState>>& dynamicState)
        :
        shaders(shaders),
        vertexInput(vertexInput),
        inputAssembly(inputAssembly),
        tesselation(tesselation),
        viewportState(viewportState),
        rasterizer(rasterizer),
        multisampling(multisampling),
        depthStencil(depthStencil),
        colorBlending(colorBlending),
        dynamicState(dynamicState),
        setLayoutBindings(setLayoutBindings),
        parentPipelineIndex(-1),
        parentPipeline(parentPipeline)
    {}
}