#include "Handle.h"
#include "Attachment.h"
#include "GraphicsPipeline.h"
#include <vector>

namespace vg
{
    struct SubpassDependency
    {};

    struct Subpass
    {
        std::vector<AttachmentReference> inputAttachments;
        std::vector<AttachmentReference> colorAttachments;
        std::vector<AttachmentReference> resolveAttachments;
        AttachmentReference* depthStancilAttachment;
        std::vector<uint32_t> preserveAttachments;
        GraphicsPipeline pipeline;

        Subpass(
            GraphicsPipeline pipeline,
            std::vector<AttachmentReference> inputAttachments = {},
            std::vector<AttachmentReference> colorAttachments = {},
            std::vector<AttachmentReference> resolveAttachments = {},
            AttachmentReference* depthStancilAttachment = nullptr,
            std::vector<uint32_t> preserveAttachments = {}
        );

        // #ifdef VULKAN_HPP
        //         operator vk::Subpass() const;
        // #elif VULKAN_H_
        //         operator VkSubpass() const;
        // #endif
    };
}
