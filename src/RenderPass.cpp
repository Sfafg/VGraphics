#include <vulkan/vulkan.hpp>
#include "RenderPass.h"
#include "Device.h"
namespace vg
{
    RenderPass::RenderPass(Span<const Attachment> attachments, std::span<Subpass> subpasses, Span<const SubpassDependency> dependencies, PipelineCacheHandle cache)
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
        vk::RenderPassCreateInfo renderPassInfo({}, colorAttachments, subpassDescriptions, *(Span<const vk::SubpassDependency>*) & dependencies);
        m_handle = ((DeviceHandle) *currentDevice).createRenderPass(renderPassInfo);

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
            const GraphicsPipeline& pipeline = subpasses.begin()[i].graphicsPipeline;
            GraphicsPipeline::CreateInfo& createInfo = *pipeline.m_createInfo;

            const GraphicsPipeline* parentPipeline = createInfo.parent;
            if (createInfo.parentIndex != -1)
                parentPipeline = &subpasses.begin()[createInfo.parentIndex].graphicsPipeline;

            shaderStages[i].resize(createInfo.GetShaders(parentPipeline)->size());
            for (int j = 0; j < createInfo.GetShaders(parentPipeline)->size(); j++)
                shaderStages[i][j] = *(*createInfo.GetShaders(parentPipeline))[j];

            auto* vertexInput = (vk::PipelineVertexInputStateCreateInfo*) createInfo.GetVertexInput(parentPipeline);
            auto* inputAssembly = (vk::PipelineInputAssemblyStateCreateInfo*) createInfo.GetInputAssembly(parentPipeline);
            auto* tesselation = (vk::PipelineTessellationStateCreateInfo*) createInfo.GetTesselation(parentPipeline);
            auto* viewportState = (vk::PipelineViewportStateCreateInfo*) createInfo.GetViewportState(parentPipeline);
            auto* rasterizer = (vk::PipelineRasterizationStateCreateInfo*) createInfo.GetRasterizer(parentPipeline);
            auto* multisampling = (vk::PipelineMultisampleStateCreateInfo*) createInfo.GetMultisampling(parentPipeline);
            auto* depthStencil = (vk::PipelineDepthStencilStateCreateInfo*) createInfo.GetDepthStencil(parentPipeline);
            auto* colorBlending = (vk::PipelineColorBlendStateCreateInfo*) createInfo.GetColorBlending(parentPipeline);

            dynamicStates[i] = vk::PipelineDynamicStateCreateInfo({}, *(std::vector<vk::DynamicState>*) createInfo.GetDynamicState(parentPipeline));

            std::vector<DescriptorSetLayoutHandle> descriptorSetLayouts(createInfo.GetSetLayoutBindings(parentPipeline)->size());
            for (int i = 0; i < descriptorSetLayouts.size(); i++)
                descriptorSetLayouts[i] = (((DeviceHandle) *currentDevice).createDescriptorSetLayout({ {}, *(std::vector<vk::DescriptorSetLayoutBinding>*) & createInfo.GetSetLayoutBindings(parentPipeline)[0][i] }));

            PipelineLayoutHandle layout = ((DeviceHandle) *currentDevice).createPipelineLayout(vk::PipelineLayoutCreateInfo({}, descriptorSetLayouts, *(std::vector<vk::PushConstantRange>*)createInfo.GetPushConstantRanges(parentPipeline)));
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
                (GraphicsPipelineHandle) (createInfo.parent ? *createInfo.parent : GraphicsPipelineHandle()), createInfo.parentIndex
            );

            createInfo.DecrementParentInheritance();
        }

        vkCreateGraphicsPipelines((DeviceHandle) *currentDevice, (PipelineCacheHandle) cache, graphicPipelineCreateInfos.size(), (VkGraphicsPipelineCreateInfo*) graphicPipelineCreateInfos.data(), nullptr, (VkPipeline*) m_graphicsPipelines.data());
        for (unsigned int i = 0; i < subpasses.size(); i++)
        {
            GraphicsPipeline::CreateInfo& createInfo = *subpasses.begin()[i].graphicsPipeline.m_createInfo;
            createInfo.UpdateParentInheritance();
            if (createInfo.inheritanceCount == 0)
            {
                delete subpasses.begin()[i].graphicsPipeline.m_createInfo;
                subpasses.begin()[i].graphicsPipeline.m_createInfo = nullptr;
            }
        }
    }


    RenderPass::RenderPass(Span<const Attachment> attachments, std::initializer_list<Subpass> subpasses, Span<const SubpassDependency> dependencies, PipelineCacheHandle cache)
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
        vk::RenderPassCreateInfo renderPassInfo({}, colorAttachments, subpassDescriptions, *(Span<const vk::SubpassDependency>*) & dependencies);
        m_handle = ((DeviceHandle) *currentDevice).createRenderPass(renderPassInfo);

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
            const GraphicsPipeline& pipeline = subpasses.begin()[i].graphicsPipeline;
            GraphicsPipeline::CreateInfo& createInfo = *pipeline.m_createInfo;

            const GraphicsPipeline* parentPipeline = createInfo.parent;
            if (createInfo.parentIndex != -1)
                parentPipeline = &subpasses.begin()[createInfo.parentIndex].graphicsPipeline;

            shaderStages[i].resize(createInfo.GetShaders(parentPipeline)->size());
            for (int j = 0; j < createInfo.GetShaders(parentPipeline)->size(); j++)
                shaderStages[i][j] = *(*createInfo.GetShaders(parentPipeline))[j];

            auto* vertexInput = (vk::PipelineVertexInputStateCreateInfo*) createInfo.GetVertexInput(parentPipeline);
            auto* inputAssembly = (vk::PipelineInputAssemblyStateCreateInfo*) createInfo.GetInputAssembly(parentPipeline);
            auto* tesselation = (vk::PipelineTessellationStateCreateInfo*) createInfo.GetTesselation(parentPipeline);
            auto* viewportState = (vk::PipelineViewportStateCreateInfo*) createInfo.GetViewportState(parentPipeline);
            auto* rasterizer = (vk::PipelineRasterizationStateCreateInfo*) createInfo.GetRasterizer(parentPipeline);
            auto* multisampling = (vk::PipelineMultisampleStateCreateInfo*) createInfo.GetMultisampling(parentPipeline);
            auto* depthStencil = (vk::PipelineDepthStencilStateCreateInfo*) createInfo.GetDepthStencil(parentPipeline);
            auto* colorBlending = (vk::PipelineColorBlendStateCreateInfo*) createInfo.GetColorBlending(parentPipeline);

            dynamicStates[i] = vk::PipelineDynamicStateCreateInfo({}, *(std::vector<vk::DynamicState>*) createInfo.GetDynamicState(parentPipeline));

            std::vector<DescriptorSetLayoutHandle> descriptorSetLayouts(createInfo.GetSetLayoutBindings(parentPipeline)->size());
            for (int i = 0; i < descriptorSetLayouts.size(); i++)
                descriptorSetLayouts[i] = (((DeviceHandle) *currentDevice).createDescriptorSetLayout({ {}, *(std::vector<vk::DescriptorSetLayoutBinding>*) & createInfo.GetSetLayoutBindings(parentPipeline)[0][i] }));

            PipelineLayoutHandle layout = ((DeviceHandle) *currentDevice).createPipelineLayout(vk::PipelineLayoutCreateInfo({}, descriptorSetLayouts, *(std::vector<vk::PushConstantRange>*)createInfo.GetPushConstantRanges(parentPipeline)));
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
                (GraphicsPipelineHandle) (createInfo.parent ? *createInfo.parent : GraphicsPipelineHandle()), createInfo.parentIndex
            );
        }
        vkCreateGraphicsPipelines((DeviceHandle) *currentDevice, (PipelineCacheHandle) cache, graphicPipelineCreateInfos.size(), (VkGraphicsPipelineCreateInfo*) graphicPipelineCreateInfos.data(), nullptr, (VkPipeline*) m_graphicsPipelines.data());
    }

    RenderPass::RenderPass() :m_handle(nullptr) {}

    RenderPass::RenderPass(RenderPass&& other) noexcept
        :RenderPass()
    {
        *this = std::move(other);
    }

    RenderPass::~RenderPass()
    {
        if (m_handle == nullptr) return;

        for (auto& layout : m_pipelineLayouts)
        {
            for (const auto& descriptor : layout.GetDescriptorSets())
                ((DeviceHandle) *currentDevice).destroyDescriptorSetLayout(descriptor);
            ((DeviceHandle) *currentDevice).destroyPipelineLayout(layout);
        }
        for (auto&& pipeline : m_graphicsPipelines)
        {
            ((DeviceHandle) *currentDevice).destroyPipeline(pipeline);
        }

        m_graphicsPipelines.clear();
        m_graphicsPipelines.shrink_to_fit();
        m_pipelineLayouts.clear();
        m_pipelineLayouts.shrink_to_fit();

        ((DeviceHandle) *currentDevice).destroyRenderPass(m_handle);
        m_handle = nullptr;
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

    Span<const PipelineLayout> RenderPass::GetPipelineLayouts() const
    {
        return m_pipelineLayouts;
    }

    Span<GraphicsPipelineHandle> RenderPass::GetPipelines()
    {
        return m_graphicsPipelines;
    }

    Span<const GraphicsPipelineHandle> RenderPass::GetPipelines() const
    {
        return m_graphicsPipelines;
    }
}