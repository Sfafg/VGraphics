#include <vulkan/vulkan.hpp>
#include "GraphicsPipeline.h"
#include "Device.h"
#include <iostream>

namespace vg {
GraphicsPipeline::GraphicsPipeline(
    uint32_t pipelineLayout, const std::vector<Shader *> &shaders, const VertexLayout &vertexInput,
    const InputAssembly &inputAssembly, const Tesselation &tesselation, const ViewportState &viewportState,
    const Rasterizer &rasterizer, const Multisampling &multisampling, const DepthStencil &depthStencil,
    const ColorBlending &colorBlending, const std::vector<DynamicState> &dynamicState, uint32_t childrenCount
)
    : m_createInfo(new CreateInfo(
          childrenCount, pipelineLayout, shaders, vertexInput, inputAssembly, tesselation, viewportState, rasterizer,
          multisampling, depthStencil, colorBlending, dynamicState, -1, nullptr
      )),
      m_handle(nullptr) {}

GraphicsPipeline::GraphicsPipeline(
    uint32_t pipelineLayout, std::vector<Shader> &&shaders, const VertexLayout &vertexInput,
    const InputAssembly &inputAssembly, const Tesselation &tesselation, const ViewportState &viewportState,
    const Rasterizer &rasterizer, const Multisampling &multisampling, const DepthStencil &depthStencil,
    const ColorBlending &colorBlending, const std::vector<DynamicState> &dynamicState, uint32_t childrenCount
)
    : m_createInfo(new CreateInfo(
          childrenCount, pipelineLayout, std::move(shaders), vertexInput, inputAssembly, tesselation, viewportState,
          rasterizer, multisampling, depthStencil, colorBlending, dynamicState, -1, nullptr
      )),
      m_handle(nullptr) {}

GraphicsPipeline::GraphicsPipeline(
    uint32_t parentIndex, std::optional<uint32_t> pipelineLayout, const std::optional<std::vector<Shader *>> &shaders,
    const std::optional<VertexLayout> &vertexInput, const std::optional<InputAssembly> &inputAssembly,
    const std::optional<Tesselation> &tesselation, const std::optional<ViewportState> &viewportState,
    const std::optional<Rasterizer> &rasterizer, const std::optional<Multisampling> &multisampling,
    const std::optional<DepthStencil> &depthStencil, const std::optional<ColorBlending> &colorBlending,
    const std::optional<std::vector<DynamicState>> &dynamicState, uint32_t childrenCount
)
    : m_createInfo(new CreateInfo(
          childrenCount, pipelineLayout, shaders, vertexInput, inputAssembly, tesselation, viewportState, rasterizer,
          multisampling, depthStencil, colorBlending, dynamicState, parentIndex, nullptr
      )),
      m_handle(nullptr) {}

GraphicsPipeline::GraphicsPipeline(
    uint32_t parentIndex, std::optional<uint32_t> pipelineLayout, std::optional<std::vector<Shader>> &&shaders,
    const std::optional<VertexLayout> &vertexInput, const std::optional<InputAssembly> &inputAssembly,
    const std::optional<Tesselation> &tesselation, const std::optional<ViewportState> &viewportState,
    const std::optional<Rasterizer> &rasterizer, const std::optional<Multisampling> &multisampling,
    const std::optional<DepthStencil> &depthStencil, const std::optional<ColorBlending> &colorBlending,
    const std::optional<std::vector<DynamicState>> &dynamicState, uint32_t childrenCount
)
    : m_createInfo(new CreateInfo(
          childrenCount, pipelineLayout, std::move(shaders), vertexInput, inputAssembly, tesselation, viewportState,
          rasterizer, multisampling, depthStencil, colorBlending, dynamicState, parentIndex, nullptr
      )),
      m_handle(nullptr) {}

GraphicsPipeline::GraphicsPipeline(
    GraphicsPipeline *parent, std::optional<uint32_t> pipelineLayout,
    const std::optional<std::vector<Shader *>> &shaders, const std::optional<VertexLayout> &vertexInput,
    const std::optional<InputAssembly> &inputAssembly, const std::optional<Tesselation> &tesselation,
    const std::optional<ViewportState> &viewportState, const std::optional<Rasterizer> &rasterizer,
    const std::optional<Multisampling> &multisampling, const std::optional<DepthStencil> &depthStencil,
    const std::optional<ColorBlending> &colorBlending, const std::optional<std::vector<DynamicState>> &dynamicState,
    uint32_t childrenCount
)
    : m_createInfo(new CreateInfo(
          childrenCount, pipelineLayout, shaders, vertexInput, inputAssembly, tesselation, viewportState, rasterizer,
          multisampling, depthStencil, colorBlending, dynamicState, -1, parent
      )),
      m_handle(nullptr) {}

GraphicsPipeline::GraphicsPipeline(
    GraphicsPipeline *parent, std::optional<uint32_t> pipelineLayout, std::optional<std::vector<Shader>> &&shaders,
    const std::optional<VertexLayout> &vertexInput, const std::optional<InputAssembly> &inputAssembly,
    const std::optional<Tesselation> &tesselation, const std::optional<ViewportState> &viewportState,
    const std::optional<Rasterizer> &rasterizer, const std::optional<Multisampling> &multisampling,
    const std::optional<DepthStencil> &depthStencil, const std::optional<ColorBlending> &colorBlending,
    const std::optional<std::vector<DynamicState>> &dynamicState, uint32_t childrenCount
)
    : m_createInfo(new CreateInfo(
          childrenCount, pipelineLayout, std::move(shaders), vertexInput, inputAssembly, tesselation, viewportState,
          rasterizer, multisampling, depthStencil, colorBlending, dynamicState, -1, parent
      )),
      m_handle(nullptr) {}

GraphicsPipeline::GraphicsPipeline() : m_handle(nullptr), m_createInfo(nullptr) {}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&other) noexcept : GraphicsPipeline() { *this = std::move(other); }

GraphicsPipeline::~GraphicsPipeline() {
    if (m_createInfo) {
        delete m_createInfo;
        m_createInfo = nullptr;
    }
#ifdef VG_DEBUG
    if (m_createInfo && m_createInfo->inheritanceCount > 0) {
        std::cout << "Not all pipeline children used, please use all children or lower the children count, Count: "
                  << m_createInfo->inheritanceCount << '\n';
    }
#elif VG_NDEBUG
#elif NDEBUG
#else
    if (m_createInfo && m_createInfo->inheritanceCount > 0) {
        std::cout << "Not all pipeline children used, please use all children or lower the children count, Count: "
                  << m_createInfo->inheritanceCount << '\n';
    }
#endif
}

GraphicsPipeline &GraphicsPipeline::operator=(GraphicsPipeline &&other) noexcept {
    if (&other == this) return *this;

    std::swap(m_handle, other.m_handle);
    std::swap(m_createInfo, other.m_createInfo);

    return *this;
}

GraphicsPipeline::operator const GraphicsPipelineHandle &() const { return m_handle; }

const uint32_t GraphicsPipeline::CreateInfo::GetPipelineLayout(const GraphicsPipeline *parent) const {
    if (pipelineLayout) return *pipelineLayout;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetPipelineLayout(parent);

    return 0;
}

const std::vector<Shader *> *GraphicsPipeline::CreateInfo::GetShaders(const GraphicsPipeline *parent) const {
    if (shaders) return &*shaders;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetShaders(parent);

    return nullptr;
}

const VertexLayout *GraphicsPipeline::CreateInfo::GetVertexInput(const GraphicsPipeline *parent) const {
    if (vertexInput) return &*vertexInput;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetVertexInput(parent);

    return nullptr;
}

const InputAssembly *GraphicsPipeline::CreateInfo::GetInputAssembly(const GraphicsPipeline *parent) const {
    if (inputAssembly) return &*inputAssembly;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetInputAssembly(parent);

    return nullptr;
}

const Tesselation *GraphicsPipeline::CreateInfo::GetTesselation(const GraphicsPipeline *parent) const {
    if (tesselation) return &*tesselation;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetTesselation(parent);

    return nullptr;
}

const ViewportState *GraphicsPipeline::CreateInfo::GetViewportState(const GraphicsPipeline *parent) const {
    if (viewportState) return &*viewportState;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetViewportState(parent);

    return nullptr;
}

const Rasterizer *GraphicsPipeline::CreateInfo::GetRasterizer(const GraphicsPipeline *parent) const {
    if (rasterizer) return &*rasterizer;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetRasterizer(parent);

    return nullptr;
}

const Multisampling *GraphicsPipeline::CreateInfo::GetMultisampling(const GraphicsPipeline *parent) const {
    if (multisampling) return &*multisampling;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetMultisampling(parent);

    return nullptr;
}

const DepthStencil *GraphicsPipeline::CreateInfo::GetDepthStencil(const GraphicsPipeline *parent) const {
    if (depthStencil) return &*depthStencil;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetDepthStencil(parent);

    return nullptr;
}

const ColorBlending *GraphicsPipeline::CreateInfo::GetColorBlending(const GraphicsPipeline *parent) const {
    if (colorBlending) return &*colorBlending;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetColorBlending(parent);

    return nullptr;
}

const std::vector<DynamicState> *GraphicsPipeline::CreateInfo::GetDynamicState(const GraphicsPipeline *parent) const {
    if (dynamicState) return &*dynamicState;

    if (parent && parent->m_createInfo) return parent->m_createInfo->GetDynamicState(parent);

    return nullptr;
}

GraphicsPipelineHandle GraphicsPipeline::CreateInfo::GetParentHandle(const GraphicsPipeline *parent) const {
    if (parent) return parent->m_handle;

    return GraphicsPipelineHandle(nullptr);
}

void GraphicsPipeline::CreateInfo::DecrementParentInheritance() {
    if (parent && parent->m_createInfo) {
#ifdef VG_DEBUG
        if (parent->m_createInfo->inheritanceCount == 0)
            std::cout << "Trying to inherit from a pipeline, but children count is full.\n";
#elif VG_NDEBUG
#elif NDEBUG
#else
        if (parent->m_createInfo->inheritanceCount == 0)
            std::cout << "Trying to inherit from a pipeline, but children count is full.\n";
#endif
        parent->m_createInfo->inheritanceCount--;
        parent->m_createInfo->DecrementParentInheritance();
    }
}

void GraphicsPipeline::CreateInfo::UpdateParentInheritance() {
    if (parent && parent->m_createInfo) {
        if (parent->m_createInfo->inheritanceCount == 0) {
            parent->m_createInfo->UpdateParentInheritance();
            delete parent->m_createInfo;
            parent->m_createInfo = nullptr;
        }
    }
}
} // namespace vg
