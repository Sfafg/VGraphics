#include "Subpass.h"
#include <cstdint>

namespace vg
{
    Subpass::Subpass(
        GraphicsPipeline&& graphicsPipeline,
        std::vector<AttachmentReference> inputAttachments,
        std::vector<AttachmentReference> colorAttachments,
        std::vector<AttachmentReference> resolveAttachments,
        std::optional<AttachmentReference> depthStencilAttachment,
        std::vector<uint32_t> preserveAttachments)
        :graphicsPipeline(&graphicsPipeline), inputAttachments(inputAttachments), colorAttachments(colorAttachments), resolveAttachments(resolveAttachments), depthStencilAttachment(depthStencilAttachment), preserveAttachments(preserveAttachments)
    {}
}