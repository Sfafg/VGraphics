#pragma once
#include "Handle.h"
#include "Enums.h"
#include "Structs.h"
#include "Shader.h"
#include "Attachment.h"
#include <vector>

namespace vg
{
    /**
     *@brief Holds GraphicsPipeline creation info
     * Used in creating Renderpasses
     */
    struct GraphicsPipeline
    {
    public:
        std::vector<Shader*> shaders;
        VertexLayout vertexInput;
        InputAssembly inputAssembly;
        Tesselation tesselation;
        ViewportState viewportState;
        Rasterizer rasterizer;
        Multisampling multisampling;
        DepthStencil depthStencil;
        ColorBlending colorBlending;
        std::vector<DynamicState> dynamicState;
        std::vector<DescriptorSetLayoutBinding> setLayoutBindings;

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
            const std::vector<DescriptorSetLayoutBinding>& setLayoutBindings);
    };
}