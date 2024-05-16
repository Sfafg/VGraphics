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
    // GraphicsPipeline::GraphicsPipeline(
    //     RenderPass& renderPass,
    //     const std::vector<Shader*>& shaders,
    //     const VertexLayout& vertexInput,
    //     const InputAssembly& inputAssembly,
    //     const Tesselation& tesselation,
    //     const ViewportState& viewportState,
    //     const Rasterizer& rasterizer,
    //     const Multisampling& multisampling,
    //     const DepthStencil& depthStencil,
    //     const ColorBlending& colorBlending,
    //     const std::vector<DynamicState>& dynamicState,
    //     const PipelineLayout& layout,
    //     int subpassIndex)
    //     : m_renderPass(&renderPass)
    // {
    //     std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(shaders.size());

    //     for (unsigned int i = 0; i < shaders.size(); i++)
    //         shaderStages[i] = *shaders[i];

    //     vk::PipelineDynamicStateCreateInfo dynamicState_({}, *(std::vector<vk::DynamicState>*) & dynamicState);
    //     vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    //     vk::PipelineInputAssemblyStateCreateInfo inputAssembly_ = inputAssembly;
    //     vk::PipelineViewportStateCreateInfo viewportState_ = viewportState;

    //     vk::PipelineRasterizationStateCreateInfo rasterizer_ = rasterizer;
    //     vk::PipelineMultisampleStateCreateInfo multisampling_({}, vk::SampleCountFlagBits::e1, false);

    //     std::vector<vk::PipelineColorBlendAttachmentState> colorBlendattachments(renderPass.m_attachments.size());
    //     for (unsigned int i = 0; i < renderPass.m_attachments.size(); i++)
    //         colorBlendattachments[i] = (vk::PipelineColorBlendAttachmentState) renderPass.m_attachments[i].colorBlending;

    //     vk::PipelineColorBlendStateCreateInfo colorBlending_ = colorBlending;
    //     colorBlending_.attachmentCount = colorBlendattachments.size();
    //     colorBlending_.pAttachments = colorBlendattachments.data();

    //     vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 0, nullptr);
    //     vk::PipelineLayout m_layout = renderPass.m_device.createPipelineLayout(pipelineLayoutInfo);
    //     this->layout = *(PipelineLayout*) &m_layout;
    //     vk::GraphicsPipelineCreateInfo pipelineInfo({}, shaderStages, &vertexInputInfo, &inputAssembly_, nullptr, &viewportState_, &rasterizer_, &multisampling_, nullptr, &colorBlending_, &dynamicState_, m_layout, renderPass, subpassIndex);
    //     m_handle = renderPass.m_device.createGraphicsPipelines(nullptr, pipelineInfo).value[0];
    // }

    // GraphicsPipeline::GraphicsPipeline(
    //     RenderPass& renderPass,
    //     const std::initializer_list<Shader>&& shaders,
    //     const VertexLayout& vertexInput,
    //     const InputAssembly& inputAssembly,
    //     const Tesselation& tesselation,
    //     const ViewportState& viewportState,
    //     const Rasterizer& rasterizer,
    //     const Multisampling& multisampling,
    //     const DepthStencil& depthStencil,
    //     const ColorBlending& colorBlending,
    //     const std::vector<DynamicState>& dynamicState,
    //     const PipelineLayout& layout,
    //     int subpassIndex)
    //     : m_renderPass(&renderPass)
    // {
    //     std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(shaders.size());

    //     for (unsigned int i = 0; i < shaders.size(); i++)
    //         shaderStages[i] = shaders.begin()[i];

    //     vk::PipelineDynamicStateCreateInfo dynamicState_({}, *(std::vector<vk::DynamicState>*) & dynamicState);
    //     vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    //     vk::PipelineInputAssemblyStateCreateInfo inputAssembly_ = inputAssembly;
    //     vk::PipelineViewportStateCreateInfo viewportState_ = viewportState;

    //     vk::PipelineRasterizationStateCreateInfo rasterizer_ = rasterizer;
    //     vk::PipelineMultisampleStateCreateInfo multisampling_({}, vk::SampleCountFlagBits::e1, false);

    //     std::vector<vk::PipelineColorBlendAttachmentState> colorBlendattachments(renderPass.m_attachments.size());
    //     for (unsigned int i = 0; i < renderPass.m_attachments.size(); i++)
    //         colorBlendattachments[i] = (vk::PipelineColorBlendAttachmentState) renderPass.m_attachments[i].colorBlending;

    //     vk::PipelineColorBlendStateCreateInfo colorBlending_ = colorBlending;
    //     colorBlending_.attachmentCount = colorBlendattachments.size();
    //     colorBlending_.pAttachments = colorBlendattachments.data();

    //     vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 0, nullptr);
    //     // vk::PipelineLayout m_layout = renderPass.m_device.createPipelineLayout(pipelineLayoutInfo);
    //     vk::GraphicsPipelineCreateInfo pipelineInfo({}, shaderStages, &vertexInputInfo, &inputAssembly_, nullptr, &viewportState_, &rasterizer_, &multisampling_, nullptr, &colorBlending_, &dynamicState_, nullptr, renderPass, subpassIndex);
    //     m_handle = renderPass.m_device.createGraphicsPipelines(nullptr, pipelineInfo).value[0];
    // }

    // GraphicsPipeline::GraphicsPipeline() :m_handle(nullptr), m_renderPass(nullptr) {}

    // GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    // {
    //     std::swap(m_handle, other.m_handle);
    //     std::swap(m_renderPass, other.m_renderPass);
    //     std::swap(layout, other.layout);
    // }

    // GraphicsPipeline::~GraphicsPipeline()
    // {
    //     if (m_handle == nullptr) return;

    //     // m_renderPass->m_device.destroyPipelineLayout(nullptr);
    //     m_renderPass->m_device.destroyPipeline(m_handle);
    // }

    // GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept
    // {
    //     if (this == &other) return *this;
    //     std::swap(m_handle, other.m_handle);
    //     std::swap(m_renderPass, other.m_renderPass);
    //     std::swap(layout, other.layout);

    //     return *this;
    // }

    // GraphicsPipeline::operator const  GraphicsPipelineHandle& () const
    // {
    //     return m_handle;
    // }
}