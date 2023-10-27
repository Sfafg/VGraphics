#include "Subpass.h"

namespace vg
{
    Subpass::Subpass(
        GraphicsPipeline pipeline,
        std::vector<AttachmentReference> inputAttachments,
        std::vector<AttachmentReference> colorAttachments,
        std::vector<AttachmentReference> resolveAttachments,
        AttachmentReference* depthStancilAttachment,
        std::vector<uint32_t> preserveAttachments)
        : inputAttachments(inputAttachments), colorAttachments(colorAttachments), resolveAttachments(resolveAttachments), depthStancilAttachment(depthStancilAttachment), preserveAttachments(preserveAttachments), pipeline(pipeline)
    {}

    // #ifdef VULKAN_HPP
    //     Subpass::operator vk::Subpass() const
    //     {}
    // #elif VULKAN_H_
    //     Subpass::operator VkSubpass() const
    //     {}
    // #endif
}