#pragma once
#include "Handle.h"
#include "Structs.h"
#include "Shader.h"
#include <vector>
#include <iostream>

namespace vg {
class GraphicsPipeline {

  public:
    GraphicsPipeline(
        uint32_t pipelineLayout, const std::vector<Shader *> &shaders, const VertexLayout &vertexInput,
        const InputAssembly &inputAssembly, const Tesselation &tesselation, const ViewportState &viewportState,
        const Rasterizer &rasterizer, const Multisampling &multisampling, const DepthStencil &depthStencil,
        const ColorBlending &colorBlending, const std::vector<DynamicState> &dynamicState, int parentIndex = -1,
        GraphicsPipelineHandle parent = vg::GraphicsPipelineHandle()
    );

    GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline &) = default;
    GraphicsPipeline(GraphicsPipeline &&) = default;
    GraphicsPipeline &operator=(const GraphicsPipeline &) = default;
    GraphicsPipeline &operator=(GraphicsPipeline &&) = default;

    uint32_t pipelineLayout;
    std::vector<Shader *> shaders;
    VertexLayout vertexInput;
    InputAssembly inputAssembly;
    Tesselation tesselation;
    ViewportState viewportState;
    Rasterizer rasterizer;
    Multisampling multisampling;
    DepthStencil depthStencil;
    ColorBlending colorBlending;
    std::vector<DynamicState> dynamicState;
    int parentIndex;
    GraphicsPipelineHandle parent;
};
} // namespace vg
