#pragma once
#include "Handle.h"
#include "Enums.h"
#include "Structs.h"
#include "Shader.h"
#include "Attachment.h"
#include <vector>

namespace vg
{
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
        PipelineLayout layout;

    public:
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
            const PipelineLayout& layout);
    };
}