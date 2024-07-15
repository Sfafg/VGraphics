#pragma once
#include "Handle.h"
#include "Enums.h"
#include "Structs.h"
#include "Shader.h"
#include <vector>
#include <optional>

namespace vg
{
    class GraphicsPipeline
    {
    public:
        std::optional<std::vector<Shader*>> shaders;
        std::optional<VertexLayout> vertexInput;
        std::optional<InputAssembly> inputAssembly;
        std::optional<Tesselation> tesselation;
        std::optional<ViewportState> viewportState;
        std::optional<Rasterizer> rasterizer;
        std::optional<Multisampling> multisampling;
        std::optional<DepthStencil> depthStencil;
        std::optional<ColorBlending> colorBlending;
        std::optional<std::vector<DynamicState>> dynamicState;
        std::vector<DescriptorSetLayoutBinding> setLayoutBindings;
        int parentPipelineIndex;
        GraphicsPipelineHandle parentPipeline;

    public:
        /**
         *@brief Construct a new Graphics Pipeline object
         *
         * @param shaders Array of pointers to shaders
         * @param vertexInput Vertex layout information
         * @param inputAssembly Input assembly information
         * @param tesselation Tesselation information
         * @param viewportState Viewportstate information
         * @param rasterizer Rasterizer information
         * @param multisampling Multisampling information
         * @param depthStencil Depthstencil information
         * @param colorBlending Colorblending information
         * @param dynamicState Array of states that can be changed frame by frame
         * @param layout Information about layout e.g. uniforms
         */
        GraphicsPipeline(
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
            const std::vector<DynamicState>& dynamicState);

        GraphicsPipeline(
            int parentIndex,
            const std::vector<DescriptorSetLayoutBinding>& setLayoutBindings,
            const std::optional<std::vector<Shader*>>& shaders = std::nullopt,
            const std::optional<VertexLayout>& vertexInput = std::nullopt,
            const std::optional<InputAssembly>& inputAssembly = std::nullopt,
            const std::optional<Tesselation>& tesselation = std::nullopt,
            const std::optional<ViewportState>& viewportState = std::nullopt,
            const std::optional<Rasterizer>& rasterizer = std::nullopt,
            const std::optional<Multisampling>& multisampling = std::nullopt,
            const std::optional<DepthStencil>& depthStencil = std::nullopt,
            const std::optional<ColorBlending>& colorBlending = std::nullopt,
            const std::optional<std::vector<DynamicState>>& dynamicState = std::nullopt);


        GraphicsPipeline(
            GraphicsPipelineHandle parentPipeline,
            const std::vector<DescriptorSetLayoutBinding>& setLayoutBindings,
            const std::optional<std::vector<Shader*>>& shaders = std::nullopt,
            const std::optional<VertexLayout>& vertexInput = std::nullopt,
            const std::optional<InputAssembly>& inputAssembly = std::nullopt,
            const std::optional<Tesselation>& tesselation = std::nullopt,
            const std::optional<ViewportState>& viewportState = std::nullopt,
            const std::optional<Rasterizer>& rasterizer = std::nullopt,
            const std::optional<Multisampling>& multisampling = std::nullopt,
            const std::optional<DepthStencil>& depthStencil = std::nullopt,
            const std::optional<ColorBlending>& colorBlending = std::nullopt,
            const std::optional<std::vector<DynamicState>>& dynamicState = std::nullopt);
    };
}