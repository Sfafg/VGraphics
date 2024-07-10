#pragma once
#include "Handle.h"
#include "Enums.h"
#include "Flags.h"
#include <cstdint>

namespace vg
{
    /**
     *@brief Structure holding Color Blending data
     * Holds information about how to blend source and destination colors when rendering
     */
    struct ColorBlend
    {
        uint32_t blendEnable;
        BlendFactor srcColorBlendFactor;
        BlendFactor dstColorBlendFactor;
        BlendOp colorBlendOp;
        BlendFactor srcAlphaBlendFactor;
        BlendFactor dstAlphaBlendFactor;
        BlendOp alphaBlendOp;
        uint32_t colorWriteMask;

        /**
         *@brief Construct a new Color Blend object
         *
         * @param blendEnable If true blending enabled
         * @param srcColorBlendFactor Source blending factor
         * @param dstColorBlendFactor Destination blending factor
         * @param colorBlendOp Color blending operation
         * @param srcAlphaBlendFactor Source alpha blending factor
         * @param dstAlphaBlendFactor Destination alpha blending factor
         * @param alphaBlendOp Alpha blending operation
         * @param colorWriteMask Color write mask
         */
        ColorBlend(
            bool blendEnable = false,
            BlendFactor srcColorBlendFactor = BlendFactor::SrcAlpha,
            BlendFactor dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha,
            BlendOp colorBlendOp = BlendOp::Add,
            BlendFactor srcAlphaBlendFactor = BlendFactor::One,
            BlendFactor dstAlphaBlendFactor = BlendFactor::Zero,
            BlendOp alphaBlendOp = BlendOp::Add,
            Flags<ColorComponent> colorWriteMask = ColorComponent::RGBA
        );

#ifdef VULKAN_HPP
        operator vk::PipelineColorBlendAttachmentState() const;
#endif
    };

    /**
     *@brief Attachment used in Renderpasses
     * Describes an attachment that is used in Renderpass e.g. Color Atachment, Depth Atachment
     */
    struct Attachment
    {
        Format format;
        unsigned int samples;
        LoadOp loadOp;
        StoreOp storeOp;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        ImageLayout initialLayout;
        ImageLayout finalLayout;
        ColorBlend* colorBlending;

        /**
         *@brief Construct a new Attachment object
         *
         * @param format Format of the attachment data
         * @param finalLayout Final attachment data layout
         * @param initialLayout Initial attachment data layout
         * @param colorBlending Color blending information
         * @param loadOp Load operation
         * @param storeOp Store operation
         * @param stencilLoadOp Stencil load operation
         * @param stencilStoreOp Stencil store operation
         * @param samples Number of samples, used for multisampling
         */
        Attachment(
            Format format,
            ImageLayout finalLayout,
            ImageLayout initialLayout = ImageLayout::Undefined,
            ColorBlend* colorBlending = new ColorBlend(),
            LoadOp loadOp = LoadOp::Clear,
            StoreOp storeOp = StoreOp::Store,
            LoadOp stencilLoadOp = LoadOp::DontCare,
            StoreOp stencilStoreOp = StoreOp::DontCare,
            unsigned int samples = 1);

#ifdef VULKAN_HPP
        operator vk::AttachmentDescription() const;
#endif
    };

    /**
     *@brief Referance to attachment used in subpasses
     * Attachment referance is used in subpasses to speficy which attaments are used
     */
    struct AttachmentReference
    {
        unsigned int index;
        ImageLayout layout;
        /**
         *@brief Construct a new Attachment Reference object
         *
         * @param index index to the Attachment list in Renderpass
         * @param layout Image layout
         */
        AttachmentReference(unsigned int index = 0, ImageLayout layout = ImageLayout::Undefined);

#ifdef VULKAN_HPP
        operator vk::AttachmentReference() const;
#endif
    };
}