#pragma once
#include "Handle.h"
#include "Enums.h"
#include <cstdint>

namespace vg
{
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

                ColorBlend(
                        bool blendEnable = false,
                        BlendFactor srcColorBlendFactor = BlendFactor::SrcAlpha,
                        BlendFactor dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha,
                        BlendOp colorBlendOp = BlendOp::Add,
                        BlendFactor srcAlphaBlendFactor = BlendFactor::One,
                        BlendFactor dstAlphaBlendFactor = BlendFactor::Zero,
                        BlendOp alphaBlendOp = BlendOp::Add,
                        ColorComponent colorWriteMask = ColorComponent::RGBA
                );

#ifdef VULKAN_HPP
                operator vk::PipelineColorBlendAttachmentState() const;
#endif
#ifdef VULKAN_H_
                operator VkPipelineColorBlendAttachmentState() const;
#endif
        };

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
                ColorBlend colorBlending;

                Attachment(
                        Format format,
                        ImageLayout finalLayout,
                        ImageLayout initialLayout = ImageLayout::Undefined,
                        ColorBlend colorBlending = ColorBlend(),
                        LoadOp loadOp = LoadOp::Clear,
                        StoreOp storeOp = StoreOp::Store,
                        LoadOp stencilLoadOp = LoadOp::DontCare,
                        StoreOp stencilStoreOp = StoreOp::DontCare,
                        unsigned int samples = 1);

#ifdef VULKAN_HPP
                operator vk::AttachmentDescription() const;
#endif
#ifdef VULKAN_H_
                operator VkAttachmentDescription() const;
#endif
        };

        struct AttachmentReference
        {
                unsigned int index;
                ImageLayout layout;
                AttachmentReference(unsigned int index = 0, ImageLayout layout = ImageLayout::Undefined);

#ifdef VULKAN_HPP
                operator vk::AttachmentReference() const;
#endif
#ifdef VULKAN_H_
                operator VkAttachmentReference() const;
#endif
        };
}