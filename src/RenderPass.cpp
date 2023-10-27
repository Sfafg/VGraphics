#include <vulkan/vulkan.hpp>
#include "RenderPass.h"
#include "Attachment.h"
namespace vg
{
    vk::Pipeline CreatePipeline(const GraphicsPipeline& graphicsPipeline, const std::vector<Attachment>& attachments, DeviceHandle device, RenderPassHandle renderPass)
    {
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(graphicsPipeline.shaders.size());

        for (int i = 0; i < graphicsPipeline.shaders.size(); i++)
            shaderStages[i] = *graphicsPipeline.shaders[i];

        vk::PipelineDynamicStateCreateInfo dynamicState({}, *(std::vector<vk::DynamicState>*) & graphicsPipeline.dynamicState);
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly = graphicsPipeline.inputAssembly;
        vk::PipelineViewportStateCreateInfo viewportState = graphicsPipeline.viewportState;

        vk::PipelineRasterizationStateCreateInfo rasterizer = graphicsPipeline.rasterizer;
        vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1, false);

        std::vector<vk::PipelineColorBlendAttachmentState> colorBlendattachments(attachments.size());
        for (int i = 0; i < attachments.size(); i++)
            colorBlendattachments[i] = (vk::PipelineColorBlendAttachmentState) attachments[i].colorBlending;

        vk::PipelineColorBlendStateCreateInfo colorBlending = graphicsPipeline.colorBlending;
        colorBlending.attachmentCount = colorBlendattachments.size();
        colorBlending.pAttachments = colorBlendattachments.data();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 0, nullptr);
        vk::PipelineLayout m_layout = vk::Device(device).createPipelineLayout(pipelineLayoutInfo);
        vk::GraphicsPipelineCreateInfo pipelineInfo({}, shaderStages, &vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling, nullptr, &colorBlending, &dynamicState, m_layout, renderPass, 0);
        return vk::Device((vg::DeviceHandle) device).createGraphicsPipelines(nullptr, pipelineInfo).value[0];
    }

    RenderPass::RenderPass() :m_handle(nullptr), m_device(nullptr)
    {}

    RenderPass::RenderPass(DeviceHandle device, const std::vector<Attachment>& attachments, const std::vector<Subpass>& subpasses, const std::vector<SubpassDependency>& dependencies) :
        m_device(device), m_attachments(attachments), m_subpasses(subpasses), m_dependencies(dependencies)
    {
        std::vector<vk::AttachmentDescription> colorAttachments(attachments.size());
        for (int i = 0; i < attachments.size(); i++)
        {
            colorAttachments[i] = vk::AttachmentDescription({},
                (vk::Format) attachments[i].format,
                (vk::SampleCountFlagBits) attachments[i].samples,
                (vk::AttachmentLoadOp) attachments[i].loadOp,
                (vk::AttachmentStoreOp) attachments[i].storeOp,
                (vk::AttachmentLoadOp) attachments[i].stencilLoadOp,
                (vk::AttachmentStoreOp) attachments[i].stencilStoreOp,
                (vk::ImageLayout) attachments[i].initialLayout,
                (vk::ImageLayout) attachments[i].finalLayout
            );
        }

        std::vector <vk::SubpassDescription> subpasses_(subpasses.size());
        for (int i = 0; i < subpasses.size(); i++)
        {
            subpasses_[i] = vk::SubpassDescription({},
                vk::PipelineBindPoint::eGraphics,
                *(std::vector<vk::AttachmentReference>*) & subpasses[i].inputAttachments,
                *(std::vector<vk::AttachmentReference>*) & subpasses[i].colorAttachments,
                *(std::vector<vk::AttachmentReference>*) & subpasses[i].resolveAttachments,
                nullptr,
                *(std::vector<uint32_t>*) & subpasses[i].preserveAttachments
            );
        }

        VkSubpassDependency dependency_{};
        dependency_.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency_.dstSubpass = 0;
        dependency_.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency_.srcAccessMask = 0;
        dependency_.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency_.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vk::SubpassDependency dependency(dependency_);

        vk::RenderPassCreateInfo renderPassInfo({}, colorAttachments, subpasses_, dependency);
        m_handle = vk::Device((vg::DeviceHandle) device).createRenderPass(renderPassInfo);

        for (int i = 0; i < subpasses.size(); i++)
        {
            m_graphicsPipelines.push_back(CreatePipeline(subpasses[i].pipeline, attachments, device, m_handle));
        }

    }

    RenderPass::RenderPass(RenderPass&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_subpasses, other.m_subpasses);
        std::swap(m_dependencies, other.m_dependencies);
        other.m_handle = nullptr;
    }

    RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_subpasses, other.m_subpasses);
        std::swap(m_dependencies, other.m_dependencies);
        other.m_handle = nullptr;

        return *this;
    }

    RenderPass::~RenderPass()
    {
        if (m_handle == nullptr) return;
        vk::Device(m_device).destroyRenderPass(m_handle);
        for (const auto& pipeline : m_graphicsPipelines)
        {
            vk::Device(m_device).destroyPipeline(pipeline);
        }
    }

    RenderPass::operator RenderPassHandle() const
    {
        return m_handle;
    }


}