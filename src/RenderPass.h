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
        RenderPass();
        RenderPass(DeviceHandle device, const std::vector<Attachment>& attachments, const std::vector<Subpass>& subpasses, const std::vector<SubpassDependency>& dependencies);

        RenderPass(RenderPass&& other) noexcept;
        RenderPass& operator=(RenderPass&& other) noexcept;
        RenderPass(const RenderPass& other) = delete;
        RenderPass& operator=(const RenderPass& other) = delete;
        ~RenderPass();

        operator RenderPassHandle() const;
        GraphicsPipelineHandle GetGraphicsPipeline(int index = 0);

    public:
        RenderPassHandle m_handle;
        DeviceHandle m_device;
        std::vector<GraphicsPipelineHandle> m_graphicsPipelines;
        std::vector<Attachment> m_attachments;
        std::vector<Subpass> m_subpasses;
        std::vector<SubpassDependency> m_dependencies;
    };
}