#include "vulkan/vulkan.hpp"
#include "Attachment.h"

namespace vg
{
    ColorBlend::ColorBlend(
        bool blendEnable,
        BlendFactor srcColorBlendFactor,
        BlendFactor dstColorBlendFactor,
        BlendOp colorBlendOp,
        BlendFactor srcAlphaBlendFactor,
        BlendFactor dstAlphaBlendFactor,
        BlendOp alphaBlendOp,
        ColorComponent colorWriteMask)
        :
        blendEnable(blendEnable),
        srcColorBlendFactor(srcColorBlendFactor),
        dstColorBlendFactor(dstColorBlendFactor),
        colorBlendOp(colorBlendOp),
        srcAlphaBlendFactor(srcAlphaBlendFactor),
        dstAlphaBlendFactor(dstAlphaBlendFactor),
        alphaBlendOp(alphaBlendOp),
        colorWriteMask((uint32_t) colorWriteMask)
    {}

    Attachment::Attachment(
        Format format,
        ImageLayout finalLayout,
        ImageLayout initialLayout,
        ColorBlend colorBlending,
        LoadOp loadOp,
        StoreOp storeOp,
        LoadOp stencilLoadOp,
        StoreOp stencilStoreOp,
        unsigned int samples)
        :
        format(format),
        samples(samples),
        loadOp(loadOp),
        storeOp(storeOp),
        stencilLoadOp(stencilLoadOp),
        stencilStoreOp(stencilStoreOp),
        initialLayout(initialLayout),
        finalLayout(finalLayout),
        colorBlending(colorBlending)
    {}

    AttachmentReference::AttachmentReference(unsigned int index, ImageLayout layout) : index(index), layout(layout)
    {}
#ifdef VULKAN_HPP

    ColorBlend::operator vk::PipelineColorBlendAttachmentState() const
    {
        return *(vk::PipelineColorBlendAttachmentState*) this;
    }

    Attachment::operator vk::AttachmentDescription() const
    {
        return vk::AttachmentDescription(
            {},
            (vk::Format) format,
            (vk::SampleCountFlagBits) samples,
            (vk::AttachmentLoadOp) loadOp,
            (vk::AttachmentStoreOp) storeOp,
            (vk::AttachmentLoadOp) stencilLoadOp,
            (vk::AttachmentStoreOp) stencilStoreOp,
            (vk::ImageLayout) initialLayout,
            (vk::ImageLayout) finalLayout
        );
    }

    AttachmentReference::operator vk::AttachmentReference() const
    {
        return *(vk::AttachmentReference*) this;
    }
#endif
}