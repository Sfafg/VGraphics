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
        GraphicsPipeline graphicsPipeline;
        std::vector<AttachmentReference> inputAttachments;
        std::vector<AttachmentReference> colorAttachments;
        std::vector<AttachmentReference> resolveAttachments;
        AttachmentReference* depthStancilAttachment;
        std::vector<uint32_t> preserveAttachments;

        Subpass(
            GraphicsPipeline&& graphicsPipeline,
            std::vector<AttachmentReference> inputAttachments = {},
            std::vector<AttachmentReference> colorAttachments = {},
            std::vector<AttachmentReference> resolveAttachments = {},
            AttachmentReference* depthStancilAttachment = nullptr,
            std::vector<uint32_t> preserveAttachments = {}
        );
    };
}
