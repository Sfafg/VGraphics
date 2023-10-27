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
        const PipelineLayout& layout)
        :shaders(shaders), vertexInput(vertexInput), inputAssembly(inputAssembly), tesselation(tesselation), viewportState(viewportState), rasterizer(rasterizer), multisampling(multisampling), depthStencil(depthStencil), colorBlending(colorBlending), dynamicState(dynamicState), layout(layout)
    {}

    // GraphicsPipeline::GraphicsPipeline() : m_handle(nullptr), m_layout(nullptr), m_device(nullptr) {}
    // GraphicsPipeline::GraphicsPipeline(
    //     DeviceHandle device,
    //     const std::vector<Shader*>& shaders,
    //     Viewport viewportState,
    //     Scissor scissor,
    //     std::vector<Attachment> attachments,
    //     Subpass subpass,
    //     const std::vector<DynamicState>& dynamicStates,
    //     Primitive primitive,
    //     bool depthClamp,
    //     PolygonMode polygonMode,
    //     float lineWidth,
    //     CullMode cullMode,
    //     FrontFace frontFace,
    //     DepthBias depthBias,
    //     bool enableLogicOp,
    //     LogicOp logicOp,
    //     float blendConstant1,
    //     float blendConstant2,
    //     float blendConstant3,
    //     float blendConstant4
    // ) : m_device(device)
    // {
    //     std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(shaders.size());
    //     for (int i = 0; i < shaders.size(); i++)
    //     {
    //         shaderStages[i] = vk::PipelineShaderStageCreateInfo({}, (vk::ShaderStageFlagBits) shaders[i]->GetStage(), (vg::ShaderHandle) *shaders[i], "main");
    //     }
    //     vk::PipelineDynamicStateCreateInfo dynamicState({}, *(std::vector < vk::DynamicState>*) & dynamicStates);
    //     vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    //     vk::PipelineInputAssemblyStateCreateInfo inputAssembly({}, (vk::PrimitiveTopology) primitive, false);
    //     vk::PipelineViewportStateCreateInfo viewportState({}, *(vk::Viewport*) &viewportState, *(vk::Rect2D*) &scissor);
    //     vk::PipelineRasterizationStateCreateInfo rasterizer({}, depthClamp, false, (vk::PolygonMode) polygonMode, (vk::CullModeFlagBits) cullMode, (vk::FrontFace) frontFace, false, 0.0f, 0.0f, 0.0f, lineWidth);
    //     vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1, false);


    //     std::vector<vk::PipelineColorBlendAttachmentState> colorBlendattachments(attachments.size());
    //     for (int i = 0; i < attachments.size(); i++)
    //     {
    //         colorBlendattachments[i] = (vk::PipelineColorBlendAttachmentState) attachments[i].colorBlending;
    //     }
    //     vk::PipelineColorBlendStateCreateInfo colorBlending({}, enableLogicOp, (vk::LogicOp) logicOp, colorBlendattachments, { blendConstant1,blendConstant2,blendConstant3,blendConstant4 });
    //     vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 0, nullptr);
    //     m_layout = vk::Device((vg::DeviceHandle) device).createPipelineLayout(pipelineLayoutInfo);

    //     std::vector<vk::AttachmentDescription> colorAttachments(attachments.size());
    //     for (int i = 0; i < attachments.size(); i++)
    //     {
    //         colorAttachments[i] = vk::AttachmentDescription({},
    //             (vk::Format) attachments[i].format,
    //             (vk::SampleCountFlagBits) attachments[i].samples,
    //             (vk::AttachmentLoadOp) attachments[i].loadOp,
    //             (vk::AttachmentStoreOp) attachments[i].storeOp,
    //             (vk::AttachmentLoadOp) attachments[i].stencilLoadOp,
    //             (vk::AttachmentStoreOp) attachments[i].stencilStoreOp,
    //             (vk::ImageLayout) attachments[i].initialLayout,
    //             (vk::ImageLayout) attachments[i].finalLayout
    //         );
    //     }

    //     vk::SubpassDescription subpass_({},
    //         vk::PipelineBindPoint::eGraphics,
    //         *(std::vector<vk::AttachmentReference>*) & subpass.inputAttachments,
    //         *(std::vector<vk::AttachmentReference>*) & subpass.colorAttachments,
    //         *(std::vector<vk::AttachmentReference>*) & subpass.resolveAttachments,
    //         (vk::AttachmentReference*) subpass.depthStencilAttachment,
    //         subpass.preserveAttachments
    //     );
    //     VkSubpassDependency dependency_{};
    //     dependency_.srcSubpass = VK_SUBPASS_EXTERNAL;
    //     dependency_.dstSubpass = 0;
    //     dependency_.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //     dependency_.srcAccessMask = 0;
    //     dependency_.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    //     dependency_.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    //     vk::SubpassDependency dependency(dependency_);

    //     vk::RenderPassCreateInfo renderPassInfo({}, colorAttachments, subpass_, dependency);
    //     m_renderPass = vk::Device((vg::DeviceHandle) device).createRenderPass(renderPassInfo);
    //     vk::GraphicsPipelineCreateInfo pipelineInfo({}, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, nullptr, &colorBlending, &dynamicState, m_layout, m_renderPass, 0);
    //     m_handle = vk::Device((vg::DeviceHandle) device).createGraphicsPipelines(nullptr, pipelineInfo).value[0];
    // }
    // GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    // {
    //     std::swap(m_handle, other.m_handle);
    //     std::swap(m_layout, other.m_layout);
    //     std::swap(m_device, other.m_device);
    //     other.m_handle = nullptr;
    // }
    // GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept
    // {
    //     if (&other == this) return *this;
    //     std::swap(m_handle, other.m_handle);
    //     std::swap(m_layout, other.m_layout);
    //     std::swap(m_device, other.m_device);
    //     other.m_handle = nullptr;

    //     return *this;
    // }

    // GraphicsPipeline::~GraphicsPipeline()
    // {
    //     if (m_handle == nullptr) return;
    //     vk::Device(m_device).destroyPipelineLayout(m_layout);
    //     vk::Device(m_device).destroyPipeline(m_handle);

    // }

    // RenderPassHandle GraphicsPipeline::GetRenderPass() const
    // {
    //     return m_renderPass;
    // }


    // GraphicsPipeline::operator GraphicsPipelineHandle() const
    // {
    //     return m_handle;
    // }
}