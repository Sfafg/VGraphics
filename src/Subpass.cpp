#include "Subpass.h"
#include <cstdint>

namespace vg
{
    Subpass::Subpass(
        GraphicsPipeline&& graphicsPipeline,
        std::vector<AttachmentReference> inputAttachments,
        std::vector<AttachmentReference> colorAttachments,
        std::vector<AttachmentReference> resolveAttachments,
        AttachmentReference* depthStancilAttachment,
        std::vector<uint32_t> preserveAttachments)
        : graphicsPipeline(graphicsPipeline), inputAttachments(inputAttachments), colorAttachments(colorAttachments), resolveAttachments(resolveAttachments), depthStancilAttachment(depthStancilAttachment), preserveAttachments(preserveAttachments)
    {}
}