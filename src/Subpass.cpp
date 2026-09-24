#include "Subpass.h"
#include <cstdint>

namespace vg {
Subpass::Subpass(
    const GraphicsPipeline &graphicsPipeline, const std::vector<AttachmentReference> &inputAttachments,
    const std::vector<AttachmentReference> &colorAttachments,
    const std::vector<AttachmentReference> &resolveAttachments,
    const std::optional<AttachmentReference> &depthStencilAttachment, const std::vector<uint32_t> &preserveAttachments
)
    : graphicsPipeline(graphicsPipeline), inputAttachments(inputAttachments), colorAttachments(colorAttachments),
      resolveAttachments(resolveAttachments), depthStencilAttachment(depthStencilAttachment),
      preserveAttachments(preserveAttachments) {}

Subpass::Subpass() {}

} // namespace vg
