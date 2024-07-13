#include <vulkan/vulkan.hpp>
#include "Framebuffer.h"
#include "Device.h"

namespace vg
{
    Framebuffer::Framebuffer(RenderPassHandle renderPass, const std::vector<ImageViewHandle>& attachments, unsigned int width, unsigned int height, unsigned int layers)
    {
        m_handle = ((DeviceHandle) currentDevice).createFramebuffer({ {}, renderPass, *(std::vector<vk::ImageView>*) & attachments, width, height, layers });
    }

    Framebuffer::Framebuffer() :m_handle(nullptr) {}

    Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
    }

    Framebuffer::~Framebuffer()
    {
        if (m_handle == nullptr) return;
        ((DeviceHandle) currentDevice).destroyFramebuffer(m_handle);
    }

    Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);

        return *this;
    }

    Framebuffer::operator const FramebufferHandle& () const
    {
        return m_handle;
    }
}