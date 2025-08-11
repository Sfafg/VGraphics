#include "Subpass.h"
#include <cstdint>

namespace vg {
Subpass::Subpass(
    GraphicsPipeline &&graphicsPipeline, const std::vector<AttachmentReference> &inputAttachments,
    const std::vector<AttachmentReference> &colorAttachments,
    const std::vector<AttachmentReference> &resolveAttachments,
    const std::optional<AttachmentReference> &depthStencilAttachment, const std::vector<uint32_t> &preserveAttachments
)
    : graphicsPipeline(std::move(graphicsPipeline)), inputAttachments(inputAttachments),
      colorAttachments(colorAttachments), resolveAttachments(resolveAttachments),
      depthStencilAttachment(depthStencilAttachment), preserveAttachments(preserveAttachments) {}

Subpass::Subpass(
    GraphicsPipeline &&graphicsPipeline, std::vector<AttachmentReference> &&inputAttachments,
    std::vector<AttachmentReference> &&colorAttachments, std::vector<AttachmentReference> &&resolveAttachments,
    const std::optional<AttachmentReference> &depthStencilAttachment, std::vector<uint32_t> &&preserveAttachments
)
    : graphicsPipeline(std::move(graphicsPipeline)), inputAttachments(std::move(inputAttachments)),
      colorAttachments(std::move(colorAttachments)), resolveAttachments(std::move(resolveAttachments)),
      depthStencilAttachment(depthStencilAttachment), preserveAttachments(std::move(preserveAttachments)) {}

Subpass::Subpass() {}

Subpass::Subpass(Subpass &&other) noexcept : Subpass() { *this = std::move(other); }

Subpass &Subpass::operator=(Subpass &&other) noexcept {
    if (&other == this) return *this;

    std::swap(graphicsPipeline, other.graphicsPipeline);
    std::swap(inputAttachments, other.inputAttachments);
    std::swap(colorAttachments, other.colorAttachments);
    std::swap(resolveAttachments, other.resolveAttachments);
    std::swap(depthStencilAttachment, other.depthStencilAttachment);
    std::swap(preserveAttachments, other.preserveAttachments);

    return *this;
}
} // namespace vg
