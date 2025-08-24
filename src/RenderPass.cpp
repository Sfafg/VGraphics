#include <vulkan/vulkan.hpp>
#include "RenderPass.h"
#include "Device.h"

#include <iostream>
namespace vg {
RenderPass::RenderPass(
    Span<const Attachment> attachments, std::vector<PipelineLayout> &&pipelineLayouts, std::span<Subpass> subpasses,
    Span<const SubpassDependency> dependencies, PipelineCacheHandle cache
)
    : m_pipelineLayouts(std::move(pipelineLayouts)) {
    // Attachments.
    std::vector<vk::AttachmentDescription> colorAttachments(attachments.size());
    for (unsigned int i = 0; i < attachments.size(); i++) {
        colorAttachments[i] = vk::AttachmentDescription(
            {}, (vk::Format)attachments[i].format, (vk::SampleCountFlagBits)attachments[i].samples,
            (vk::AttachmentLoadOp)attachments[i].loadOp, (vk::AttachmentStoreOp)attachments[i].storeOp,
            (vk::AttachmentLoadOp)attachments[i].stencilLoadOp, (vk::AttachmentStoreOp)attachments[i].stencilStoreOp,
            (vk::ImageLayout)attachments[i].initialLayout, (vk::ImageLayout)attachments[i].finalLayout
        );
    }

    // Subpass descriptions.
    std::vector<vk::SubpassDescription> subpassDescriptions(subpasses.size());
    for (unsigned int i = 0; i < subpasses.size(); i++) {
        const Subpass &pass = subpasses.begin()[i];
        subpassDescriptions[i] = vk::SubpassDescription(
            {}, vk::PipelineBindPoint::eGraphics, *(std::vector<vk::AttachmentReference> *)&pass.inputAttachments,
            *(std::vector<vk::AttachmentReference> *)&pass.colorAttachments,
            *(std::vector<vk::AttachmentReference> *)&pass.resolveAttachments,
            (vk::AttachmentReference *)(pass.depthStencilAttachment ? (void *)&*pass.depthStencilAttachment : nullptr),
            *(std::vector<uint32_t> *)&pass.preserveAttachments
        );
    }

    // RenderPass.
    vk::RenderPassCreateInfo renderPassInfo(
        {}, colorAttachments, subpassDescriptions, *(Span<const vk::SubpassDependency> *)&dependencies
    );
    m_handle = ((DeviceHandle)*currentDevice).createRenderPass(renderPassInfo);

    // Graphics pipelines.
    std::vector<vk::GraphicsPipelineCreateInfo> graphicPipelineCreateInfos;
    std::vector<std::vector<vk::PipelineShaderStageCreateInfo>> shaderStages;
    std::vector<vk::PipelineDynamicStateCreateInfo> dynamicStates;
    graphicPipelineCreateInfos.resize(subpasses.size());
    shaderStages.resize(subpasses.size());
    dynamicStates.resize(subpasses.size());
    m_graphicsPipelines.resize(subpasses.size());
    for (unsigned int i = 0; i < subpasses.size(); i++) {
        const Subpass &subpass = subpasses.begin()[i];
        const GraphicsPipeline &pipeline = subpasses.begin()[i].graphicsPipeline;

        shaderStages[i].resize(pipeline.shaders.size());
        for (int j = 0; j < pipeline.shaders.size(); j++) shaderStages[i][j] = *pipeline.shaders[j];

        auto *vertexInput = (vk::PipelineVertexInputStateCreateInfo *)&pipeline.vertexInput;
        auto *inputAssembly = (vk::PipelineInputAssemblyStateCreateInfo *)&pipeline.inputAssembly;
        auto *tesselation = (vk::PipelineTessellationStateCreateInfo *)&pipeline.tesselation;
        auto *viewportState = (vk::PipelineViewportStateCreateInfo *)&pipeline.viewportState;
        auto *rasterizer = (vk::PipelineRasterizationStateCreateInfo *)&pipeline.rasterizer;
        auto *multisampling = (vk::PipelineMultisampleStateCreateInfo *)&pipeline.multisampling;
        auto *depthStencil = (vk::PipelineDepthStencilStateCreateInfo *)&pipeline.depthStencil;
        auto *colorBlending = (vk::PipelineColorBlendStateCreateInfo *)&pipeline.colorBlending;

        dynamicStates[i] =
            vk::PipelineDynamicStateCreateInfo({}, *(std::vector<vk::DynamicState> *)&pipeline.dynamicState);

        vk::PipelineCreateFlags flags = {};
        flags |= vk::PipelineCreateFlagBits::eAllowDerivatives;

        if (pipeline.parent || pipeline.parentIndex) flags |= vk::PipelineCreateFlagBits::eDerivative;

        graphicPipelineCreateInfos[i] = vk::GraphicsPipelineCreateInfo(
            flags, shaderStages[i], vertexInput, inputAssembly, tesselation, viewportState, rasterizer, multisampling,
            depthStencil, colorBlending, &dynamicStates[i], m_pipelineLayouts[pipeline.pipelineLayout],
            (vk::RenderPass)m_handle, i, pipeline.parent, pipeline.parentIndex
        );
    }

    vkCreateGraphicsPipelines(
        (DeviceHandle)*currentDevice, (PipelineCacheHandle)cache, graphicPipelineCreateInfos.size(),
        (VkGraphicsPipelineCreateInfo *)graphicPipelineCreateInfos.data(), nullptr,
        (VkPipeline *)m_graphicsPipelines.data()
    );
    for (unsigned int i = 0; i < subpasses.size(); i++) {
        subpasses.begin()[i].graphicsPipeline.m_handle = m_graphicsPipelines[i];
    }
}

RenderPass::RenderPass(
    Span<const Attachment> attachments, std::vector<PipelineLayout> &&pipelineLayouts,
    std::initializer_list<Subpass> subpasses, Span<const SubpassDependency> dependencies, PipelineCacheHandle cache
)
    : m_pipelineLayouts(std::move(pipelineLayouts)) {
    // Attachments.
    std::vector<vk::AttachmentDescription> colorAttachments(attachments.size());
    for (unsigned int i = 0; i < attachments.size(); i++) {
        colorAttachments[i] = vk::AttachmentDescription(
            {}, (vk::Format)attachments[i].format, (vk::SampleCountFlagBits)attachments[i].samples,
            (vk::AttachmentLoadOp)attachments[i].loadOp, (vk::AttachmentStoreOp)attachments[i].storeOp,
            (vk::AttachmentLoadOp)attachments[i].stencilLoadOp, (vk::AttachmentStoreOp)attachments[i].stencilStoreOp,
            (vk::ImageLayout)attachments[i].initialLayout, (vk::ImageLayout)attachments[i].finalLayout
        );
    }

    // Subpass descriptions.
    std::vector<vk::SubpassDescription> subpassDescriptions(subpasses.size());
    for (unsigned int i = 0; i < subpasses.size(); i++) {
        const Subpass &pass = subpasses.begin()[i];
        subpassDescriptions[i] = vk::SubpassDescription(
            {}, vk::PipelineBindPoint::eGraphics, *(std::vector<vk::AttachmentReference> *)&pass.inputAttachments,
            *(std::vector<vk::AttachmentReference> *)&pass.colorAttachments,
            *(std::vector<vk::AttachmentReference> *)&pass.resolveAttachments,
            (vk::AttachmentReference *)(pass.depthStencilAttachment ? (void *)&*pass.depthStencilAttachment : nullptr),
            *(std::vector<uint32_t> *)&pass.preserveAttachments
        );
    }

    // RenderPass.
    vk::RenderPassCreateInfo renderPassInfo(
        {}, colorAttachments, subpassDescriptions, *(Span<const vk::SubpassDependency> *)&dependencies
    );
    m_handle = ((DeviceHandle)*currentDevice).createRenderPass(renderPassInfo);

    // Graphics pipelines.
    std::vector<vk::GraphicsPipelineCreateInfo> graphicPipelineCreateInfos;
    std::vector<std::vector<vk::PipelineShaderStageCreateInfo>> shaderStages;
    std::vector<vk::PipelineDynamicStateCreateInfo> dynamicStates;
    graphicPipelineCreateInfos.resize(subpasses.size());
    shaderStages.resize(subpasses.size());
    dynamicStates.resize(subpasses.size());
    m_graphicsPipelines.resize(subpasses.size());
    for (unsigned int i = 0; i < subpasses.size(); i++) {
        const Subpass &subpass = subpasses.begin()[i];
        const GraphicsPipeline &pipeline = subpasses.begin()[i].graphicsPipeline;

        shaderStages[i].resize(pipeline.shaders.size());
        for (int j = 0; j < pipeline.shaders.size(); j++) shaderStages[i][j] = *pipeline.shaders[j];

        auto *vertexInput = (vk::PipelineVertexInputStateCreateInfo *)&pipeline.vertexInput;
        auto *inputAssembly = (vk::PipelineInputAssemblyStateCreateInfo *)&pipeline.inputAssembly;
        auto *tesselation = (vk::PipelineTessellationStateCreateInfo *)&pipeline.tesselation;
        auto *viewportState = (vk::PipelineViewportStateCreateInfo *)&pipeline.viewportState;
        auto *rasterizer = (vk::PipelineRasterizationStateCreateInfo *)&pipeline.rasterizer;
        auto *multisampling = (vk::PipelineMultisampleStateCreateInfo *)&pipeline.multisampling;
        auto *depthStencil = (vk::PipelineDepthStencilStateCreateInfo *)&pipeline.depthStencil;
        auto *colorBlending = (vk::PipelineColorBlendStateCreateInfo *)&pipeline.colorBlending;

        dynamicStates[i] =
            vk::PipelineDynamicStateCreateInfo({}, *(std::vector<vk::DynamicState> *)&pipeline.dynamicState);

        vk::PipelineCreateFlags flags = {};
        flags |= vk::PipelineCreateFlagBits::eAllowDerivatives;

        if (pipeline.parent || pipeline.parentIndex != -1) flags |= vk::PipelineCreateFlagBits::eDerivative;

        graphicPipelineCreateInfos[i] = vk::GraphicsPipelineCreateInfo(
            flags, shaderStages[i], vertexInput, inputAssembly, tesselation, viewportState, rasterizer, multisampling,
            depthStencil, colorBlending, &dynamicStates[i], m_pipelineLayouts[pipeline.pipelineLayout],
            (vk::RenderPass)m_handle, i, pipeline.parent, pipeline.parentIndex
        );
    }
    vkCreateGraphicsPipelines(
        (DeviceHandle)*currentDevice, (PipelineCacheHandle)cache, graphicPipelineCreateInfos.size(),
        (VkGraphicsPipelineCreateInfo *)graphicPipelineCreateInfos.data(), nullptr,
        (VkPipeline *)m_graphicsPipelines.data()
    );
}

RenderPass::RenderPass() : m_handle(nullptr) {}

RenderPass::RenderPass(RenderPass &&other) noexcept : RenderPass() { *this = std::move(other); }

RenderPass::~RenderPass() {
    if (m_handle == nullptr) return;

    m_pipelineLayouts.clear();
    m_pipelineLayouts.shrink_to_fit();
    for (auto &&pipeline : m_graphicsPipelines) { ((DeviceHandle)*currentDevice).destroyPipeline(pipeline); }

    m_graphicsPipelines.clear();
    m_graphicsPipelines.shrink_to_fit();

    ((DeviceHandle)*currentDevice).destroyRenderPass(m_handle);
    m_handle = nullptr;
}

RenderPass &RenderPass::operator=(RenderPass &&other) noexcept {
    if (&other == this) return *this;

    std::swap(m_handle, other.m_handle);
    std::swap(m_graphicsPipelines, other.m_graphicsPipelines);
    std::swap(m_pipelineLayouts, other.m_pipelineLayouts);

    return *this;
}

RenderPass::operator const RenderPassHandle &() const { return m_handle; }

Span<const PipelineLayout> RenderPass::GetPipelineLayouts() const { return m_pipelineLayouts; }

Span<GraphicsPipelineHandle> RenderPass::GetPipelines() { return m_graphicsPipelines; }

Span<const GraphicsPipelineHandle> RenderPass::GetPipelines() const { return m_graphicsPipelines; }
} // namespace vg
