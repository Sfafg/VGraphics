#include <vulkan/vulkan.hpp>
#include "RenderPass.h"
namespace vg
{
    RenderPass::RenderPass(const std::vector<Attachment>& attachments, const std::initializer_list<Subpass>&& subpasses, const std::vector<SubpassDependency>& dependencies)
        : m_attachments(attachments), m_dependencies(dependencies)
    {
        std::vector<vk::AttachmentDescription> colorAttachments(attachments.size());
        for (unsigned int i = 0; i < attachments.size(); i++)
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

        std::vector <vk::SubpassDescription> subpassDescriptions(subpasses.size());
        for (unsigned int i = 0; i < subpasses.size(); i++)
        {
            const Subpass& pass = subpasses.begin()[i];
            subpassDescriptions[i] = vk::SubpassDescription({},
                vk::PipelineBindPoint::eGraphics,
                *(std::vector<vk::AttachmentReference>*) & pass.inputAttachments,
                *(std::vector<vk::AttachmentReference>*) & pass.colorAttachments,
                *(std::vector<vk::AttachmentReference>*) & pass.resolveAttachments,
                nullptr,
                *(std::vector<uint32_t>*) & pass.preserveAttachments
            );
        }

        VkSubpassDependency subpassDependency{};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        vk::SubpassDependency dependency(subpassDependency);

        vk::RenderPassCreateInfo renderPassInfo({}, colorAttachments, subpassDescriptions, dependency);
        m_handle = ((DeviceHandle) currentDevice).createRenderPass(renderPassInfo);


        std::vector<vk::GraphicsPipelineCreateInfo> graphicPipelineCreateInfos;
        std::vector<std::vector<vk::PipelineShaderStageCreateInfo>> shaderStages;
        std::vector<vk::PipelineDynamicStateCreateInfo> dynamicStates;
        graphicPipelineCreateInfos.resize(subpasses.size());
        shaderStages.resize(subpasses.size());
        dynamicStates.resize(subpasses.size());
        m_pipelineLayouts.resize(subpasses.size());
        for (unsigned int i = 0; i < subpasses.size(); i++)
        {
            const GraphicsPipeline& pipeline = subpasses.begin()[i].graphicsPipeline;
            if (pipeline.shaders)
            {
                shaderStages[i].resize(pipeline.shaders->size());
                for (int j = 0; j < pipeline.shaders->size(); j++)
                    shaderStages[i][j] = *(*pipeline.shaders)[j];
            }
            else
                shaderStages[i] = {};

            auto* vertexInput = (vk::PipelineVertexInputStateCreateInfo*) (pipeline.vertexInput ? &*pipeline.vertexInput : nullptr);
            auto* inputAssembly = (vk::PipelineInputAssemblyStateCreateInfo*) (pipeline.inputAssembly ? &*pipeline.inputAssembly : nullptr);
            auto* tesselation = (vk::PipelineTessellationStateCreateInfo*) (pipeline.tesselation ? &*pipeline.tesselation : nullptr);
            auto* viewportState = (vk::PipelineViewportStateCreateInfo*) (pipeline.viewportState ? &*pipeline.viewportState : nullptr);
            auto* rasterizer = (vk::PipelineRasterizationStateCreateInfo*) (pipeline.rasterizer ? &*pipeline.rasterizer : nullptr);
            auto* multisampling = (vk::PipelineMultisampleStateCreateInfo*) (pipeline.multisampling ? &*pipeline.multisampling : nullptr);
            auto* depthStencil = (vk::PipelineDepthStencilStateCreateInfo*) (pipeline.depthStencil ? &*pipeline.depthStencil : nullptr);
            auto* colorBlending = (vk::PipelineColorBlendStateCreateInfo*) (pipeline.colorBlending ? &*pipeline.colorBlending : nullptr);

            if (pipeline.dynamicState)
                dynamicStates[i] = vk::PipelineDynamicStateCreateInfo({}, *(std::vector<vk::DynamicState>*) & pipeline.dynamicState.value());

            std::vector<DescriptorSetLayoutHandle> descriptorSetLayouts;
            descriptorSetLayouts.push_back(((DeviceHandle) currentDevice).createDescriptorSetLayout({ {}, *(std::vector<vk::DescriptorSetLayoutBinding>*) & pipeline.setLayoutBindings }));

            PipelineLayoutHandle layout = ((DeviceHandle) currentDevice).createPipelineLayout(vk::PipelineLayoutCreateInfo({}, descriptorSetLayouts));
            m_pipelineLayouts[i].m_handle = layout;
            m_pipelineLayouts[i].m_descriptorSetLayouts = descriptorSetLayouts;

            int creationFlags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
            if (pipeline.parentPipelineIndex != -1 || pipeline.parentPipeline != nullptr)
                creationFlags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;

            graphicPipelineCreateInfos[i] = vk::GraphicsPipelineCreateInfo(
                (vk::PipelineCreateFlags) creationFlags,
                shaderStages[i], vertexInput,
                inputAssembly, tesselation, viewportState, rasterizer,
                multisampling, depthStencil, colorBlending, pipeline.dynamicState ? &dynamicStates[i] : nullptr,
                m_pipelineLayouts[i], (vk::RenderPass) m_handle, i, pipeline.parentPipeline, pipeline.parentPipelineIndex
            );
        }

        m_graphicsPipelines.resize(graphicPipelineCreateInfos.size());
        VkPipeline* ptr = (VkPipeline*) m_graphicsPipelines.data();
        vkCreateGraphicsPipelines((DeviceHandle) currentDevice, nullptr, graphicPipelineCreateInfos.size(), (VkGraphicsPipelineCreateInfo*) graphicPipelineCreateInfos.data(), nullptr, ptr);
    }

    RenderPass::RenderPass() :m_handle(nullptr) {}

    RenderPass::RenderPass(RenderPass&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);

        std::swap(m_graphicsPipelines, other.m_graphicsPipelines);
        std::swap(m_pipelineLayouts, other.m_pipelineLayouts);
        std::swap(m_attachments, other.m_attachments);
        std::swap(m_dependencies, other.m_dependencies);
    }

    RenderPass::~RenderPass()
    {
        if (m_handle == nullptr) return;

        for (const auto& pipeline : m_graphicsPipelines) ((DeviceHandle) currentDevice).destroyPipeline(pipeline);
        for (const auto& layout : m_pipelineLayouts)
        {
            for (const auto& descriptor : layout.GetDescriptorSets())
                ((DeviceHandle) currentDevice).destroyDescriptorSetLayout(descriptor);
            ((DeviceHandle) currentDevice).destroyPipelineLayout(layout);
        }

        ((DeviceHandle) currentDevice).destroyRenderPass(m_handle);
    }

    RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
    {
        if (&other == this) return *this;

        std::swap(m_handle, other.m_handle);

        std::swap(m_graphicsPipelines, other.m_graphicsPipelines);
        std::swap(m_pipelineLayouts, other.m_pipelineLayouts);
        std::swap(m_attachments, other.m_attachments);
        std::swap(m_dependencies, other.m_dependencies);

        return *this;
    }

    RenderPass::operator const RenderPassHandle& () const
    {
        return m_handle;
    }

    const std::vector<PipelineLayout>& RenderPass::GetPipelineLayouts() const
    {
        return m_pipelineLayouts;
    }

    const std::vector<GraphicsPipelineHandle>& RenderPass::GetPipelines() const
    {
        return m_graphicsPipelines;
    }

}