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
        GraphicsPipeline *parent = nullptr
    );

    GraphicsPipeline(
        uint32_t pipelineLayout, std::vector<Shader> &&shaders, const VertexLayout &vertexInput,
        const InputAssembly &inputAssembly, const Tesselation &tesselation, const ViewportState &viewportState,
        const Rasterizer &rasterizer, const Multisampling &multisampling, const DepthStencil &depthStencil,
        const ColorBlending &colorBlending, const std::vector<DynamicState> &dynamicState, int parentIndex = -1,
        GraphicsPipeline *parent = nullptr
    );

    GraphicsPipeline();
    GraphicsPipeline(GraphicsPipeline &&other) noexcept;
    GraphicsPipeline(const GraphicsPipeline &other) = delete;

    GraphicsPipeline &operator=(GraphicsPipeline &&other) noexcept;
    GraphicsPipeline &operator=(const GraphicsPipeline &other) = delete;

    operator const GraphicsPipelineHandle &() const;

  private:
    const std::vector<Shader *> *GetShaders(const GraphicsPipeline *parent) const;
    uint32_t pipelineLayout;
    std::vector<Shader *> shaders;
    std::vector<Shader> shaders_;
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

    GraphicsPipelineHandle m_handle;
    friend class RenderPass;
};
} // namespace vg
