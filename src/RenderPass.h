#pragma once
#include "Handle.h"
#include "Attachment.h"
#include "Subpass.h"
#include "Device.h"
#include <vector>

namespace vg
{
    class RenderPass
    {
    public:
        RenderPass(const Device& device, const std::vector<Attachment>& attachments, const std::initializer_list<Subpass>&& subpasses, const std::vector<SubpassDependency>& dependencies);

        RenderPass();
        RenderPass(RenderPass&& other) noexcept;
        RenderPass(const RenderPass& other) = delete;
        ~RenderPass();

        RenderPass& operator=(RenderPass&& other) noexcept;
        RenderPass& operator=(const RenderPass& other) = delete;
        operator const RenderPassHandle& () const;

    public:
        RenderPassHandle m_handle;
        DeviceHandle m_device;
        std::vector<GraphicsPipelineHandle> m_graphicsPipelines;
        std::vector<PipelineLayoutHandle> m_pipelineLayouts;
        std::vector<Attachment> m_attachments;
        std::vector<SubpassDependency> m_dependencies;
    };
}