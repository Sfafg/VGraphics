#include <vulkan/vulkan.hpp>
#include "RenderPass.h"
#include "Device.h"
namespace vg
{
    RenderPass::RenderPass(const std::vector<Attachment>& attachments, const std::initializer_list<Subpass>&& subpasses, const std::vector<SubpassDependency>& dependencies, PipelineCacheHandle cache)
    {
        // Attachments.
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

        // Subpass descriptions.
        std::vector <vk::SubpassDescription> subpassDescriptions(subpasses.size());
        for (unsigned int i = 0; i < subpasses.size(); i++)
        {
            const Subpass& pass = subpasses.begin()[i];
            subpassDescriptions[i] = vk::SubpassDescription({},
                vk::PipelineBindPoint::eGraphics,
                *(std::vector<vk::AttachmentReference>*) & pass.inputAttachments,
                *(std::vector<vk::AttachmentReference>*) & pass.colorAttachments,
                *(std::vector<vk::AttachmentReference>*) & pass.resolveAttachments,
                (vk::AttachmentReference*) (pass.depthStencilAttachment ? (void*) &*pass.depthStencilAttachment : nullptr),
                *(std::vector<uint32_t>*) & pass.preserveAttachments
            );
        }

        // RenderPass.
        vk::RenderPassCreateInfo renderPassInfo({}, colorAttachments, subpassDescriptions, *(std::vector<vk::SubpassDependency>*) & dependencies);
        m_handle = ((DeviceHandle) currentDevice).createRenderPass(renderPassInfo);

        // Graphics pipelines.
        std::vector<vk::GraphicsPipelineCreateInfo> graphicPipelineCreateInfos;
        std::vector<std::vector<vk::PipelineShaderStageCreateInfo>> shaderStages;
        std::vector<vk::PipelineDynamicStateCreateInfo> dynamicStates;
        graphicPipelineCreateInfos.resize(subpasses.size());
        shaderStages.resize(subpasses.size());
        dynamicStates.resize(subpasses.size());
        m_pipelineLayouts.resize(subpasses.size());
        m_graphicsPipelines.resize(subpasses.size());
        for (unsigned int i = 0; i < subpasses.size(); i++)
        {
            const Subpass& subpass = subpasses.begin()[i];
            const GraphicsPipeline& pipeline = *subpasses.begin()[i].graphicsPipeline;
            GraphicsPipeline::CreateInfo& createInfo = *pipeline.m_createInfo;

            if (createInfo.parentIndex != -1)
                createInfo.parent = &m_graphicsPipelines[createInfo.parentIndex];

            shaderStages[i].resize(createInfo.GetShaders()->size());
            for (int j = 0; j < createInfo.GetShaders()->size(); j++)
                shaderStages[i][j] = *(*createInfo.GetShaders())[j];

            auto* vertexInput = (vk::PipelineVertexInputStateCreateInfo*) createInfo.GetVertexInput();
            auto* inputAssembly = (vk::PipelineInputAssemblyStateCreateInfo*) createInfo.GetInputAssembly();
            auto* tesselation = (vk::PipelineTessellationStateCreateInfo*) createInfo.GetTesselation();
            auto* viewportState = (vk::PipelineViewportStateCreateInfo*) createInfo.GetViewportState();
            auto* rasterizer = (vk::PipelineRasterizationStateCreateInfo*) createInfo.GetRasterizer();
            auto* multisampling = (vk::PipelineMultisampleStateCreateInfo*) createInfo.GetMultisampling();
            auto* depthStencil = (vk::PipelineDepthStencilStateCreateInfo*) createInfo.GetDepthStencil();
            auto* colorBlending = (vk::PipelineColorBlendStateCreateInfo*) createInfo.GetColorBlending();

            dynamicStates[i] = vk::PipelineDynamicStateCreateInfo({}, *(std::vector<vk::DynamicState>*) createInfo.GetDynamicState());

            std::vector<DescriptorSetLayoutHandle> descriptorSetLayouts;
            descriptorSetLayouts.resize(1);
            descriptorSetLayouts[0] = (((DeviceHandle) currentDevice).createDescriptorSetLayout({ {}, *(std::vector<vk::DescriptorSetLayoutBinding>*) createInfo.GetSetLayoutBindings() }));

            PipelineLayoutHandle layout = ((DeviceHandle) currentDevice).createPipelineLayout(vk::PipelineLayoutCreateInfo({}, descriptorSetLayouts, *(std::vector<vk::PushConstantRange>*)createInfo.GetPushConstantRanges()));
            m_pipelineLayouts[i].m_handle = layout;
            m_pipelineLayouts[i].m_descriptorSetLayouts = descriptorSetLayouts;

            vk::PipelineCreateFlags flags = {};
            if (createInfo.inheritanceCount > 0)
                flags |= vk::PipelineCreateFlagBits::eAllowDerivatives;

            if (createInfo.parent)
                flags |= vk::PipelineCreateFlagBits::eDerivative;

            graphicPipelineCreateInfos[i] = vk::GraphicsPipelineCreateInfo(
                flags,
                shaderStages[i], vertexInput,
                inputAssembly, tesselation, viewportState, rasterizer,
                multisampling, depthStencil, colorBlending, &dynamicStates[i],
                m_pipelineLayouts[i], (vk::RenderPass) m_handle, i,
                createInfo.parentIndex == -1 ? createInfo.GetParentHandle() : (GraphicsPipelineHandle)nullptr, createInfo.parentIndex
            );

            createInfo.DecrementParentInheritance();
            std::swap(m_graphicsPipelines[i], *subpasses.begin()[i].graphicsPipeline);
        }

        std::vector<vk::Pipeline> ptr = ((DeviceHandle) currentDevice).createGraphicsPipelines(cache, graphicPipelineCreateInfos).value;

        for (int i = 0; i < m_graphicsPipelines.size(); i++)
        {
            m_graphicsPipelines[i].m_handle = ptr[i];

            GraphicsPipeline::CreateInfo& createInfo = *m_graphicsPipelines[i].m_createInfo;
            createInfo.UpdateParentInheritance();

            if (createInfo.inheritanceCount == 0)
            {
                delete m_graphicsPipelines[i].m_createInfo;
                m_graphicsPipelines[i].m_createInfo = nullptr;
            }
        }
    }

    RenderPass::RenderPass() :m_handle(nullptr) {}

    RenderPass::RenderPass(RenderPass&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_graphicsPipelines, other.m_graphicsPipelines);
        std::swap(m_pipelineLayouts, other.m_pipelineLayouts);
    }

    RenderPass::~RenderPass()
    {
        if (m_handle == nullptr) return;

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

    std::vector<GraphicsPipeline>& RenderPass::GetPipelines()
    {
        return m_graphicsPipelines;
    }

    const std::vector<GraphicsPipeline>& RenderPass::GetPipelines() const
    {
        return m_graphicsPipelines;
    }
}