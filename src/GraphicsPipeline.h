#pragma once
#include "Handle.h"
#include "VulkanEnums.h"
#include "VulkanStructs.h"
#include "Shader.h"
#include "Attachment.h"
#include <vector>

namespace vg
{
    struct DepthBias
    {
        float constantFactor;
        float clamp;
        float slopeFactor;
        DepthBias(float constantFactor = 0, float clamp = 0, float slopeFactor = 0) :constantFactor(constantFactor), clamp(clamp), slopeFactor(slopeFactor) {}
    };
    struct VertexLayout {};
    struct InputAssembly
    {
        Primitive primitive;
        bool primitiveRestart;
        InputAssembly(Primitive primitive = Primitive::Triangles, bool primitiveRestart = false) :primitive(primitive), primitiveRestart(primitiveRestart) {}

#ifdef VULKAN_HPP
        operator vk::PipelineInputAssemblyStateCreateInfo() const
        {
            return vk::PipelineInputAssemblyStateCreateInfo({}, (vk::PrimitiveTopology) primitive, primitiveRestart);
        }
#endif
    };
    struct Tesselation {};
    struct ViewportState
    {
        Viewport viewport;
        Scissor scissor;
        ViewportState(Viewport viewport, Scissor scissor) :viewport(viewport), scissor(scissor) {}
#ifdef VULKAN_HPP
        operator vk::PipelineViewportStateCreateInfo() const
        {
            return vk::PipelineViewportStateCreateInfo({}, *(vk::Viewport*) &viewport, *(vk::Rect2D*) &scissor);
        }
#endif
    };
    struct Rasterizer
    {
        bool discard;
        bool depthClamp;
        PolygonMode polygonMode;
        CullMode cullMode;
        FrontFace frontFace;
        DepthBias depthBias;
        float lineWidth;

        Rasterizer(bool enable, bool depthClamp, PolygonMode polygonMode, CullMode cullMode, FrontFace frontFace, DepthBias depthBias, float lineWidth)
            :discard(enable), depthClamp(depthClamp), polygonMode(polygonMode), cullMode(cullMode), frontFace(frontFace), depthBias(depthBias), lineWidth(lineWidth)
        {}

#ifdef VULKAN_HPP
        operator vk::PipelineRasterizationStateCreateInfo() const
        {
            return vk::PipelineRasterizationStateCreateInfo({}, depthClamp, discard, (vk::PolygonMode) polygonMode, (vk::CullModeFlagBits) cullMode, (vk::FrontFace) frontFace, false, depthBias.constantFactor, depthBias.clamp, depthBias.slopeFactor, lineWidth);
        }
#endif
    };
    struct Multisampling {};
    struct DepthStencil {};
    struct ColorBlending
    {
        uint32_t enableLogicOp;
        LogicOp logicOp;
        float blendConsts[4];
        ColorBlending(bool enableLogicOp, LogicOp logicOp, const float(&blendConstants)[4])
            :enableLogicOp(enableLogicOp), logicOp(logicOp), blendConsts{ blendConstants[0],blendConstants[1],blendConstants[2],blendConstants[3] }
        {}

#ifdef VULKAN_HPP
        operator vk::PipelineColorBlendStateCreateInfo() const
        {
            return vk::PipelineColorBlendStateCreateInfo({}, enableLogicOp, (vk::LogicOp) logicOp, 0, nullptr, { blendConsts[0],blendConsts[1],blendConsts[2],blendConsts[3] });
        }
#endif
    };
    struct PipelineLayout {};

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