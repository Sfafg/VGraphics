#pragma once
#include "Handle.h"
#include <vector>

namespace vg
{
    class Framebuffer
    {
    public:
        Framebuffer();
        Framebuffer(DeviceHandle device, RenderPassHandle renderPass, const std::vector<ImageViewHandle>& attachments, unsigned int width, unsigned int height, unsigned int layers = 1);

        Framebuffer(Framebuffer&& other) noexcept;
        Framebuffer& operator=(Framebuffer&& other) noexcept;
        Framebuffer(const Framebuffer& other) = delete;
        Framebuffer& operator=(const Framebuffer& other) = delete;

        ~Framebuffer();

        operator FramebufferHandle()const;

    private:
        FramebufferHandle m_handle;
        DeviceHandle m_device;

    };
}