#include <vulkan/vulkan.hpp>
#include "Framebuffer.h"
#include "Device.h"

namespace vg
{
    Framebuffer::Framebuffer(RenderPassHandle renderPass, Span<const ImageViewHandle> attachments, unsigned int width, unsigned int height, unsigned int layers)
    {
        m_handle = ((DeviceHandle) *currentDevice).createFramebuffer({ {}, renderPass, *(Span<const vk::ImageView>*) & attachments, width, height, layers });
    }

    Framebuffer::Framebuffer() :m_handle(nullptr) {}

    Framebuffer::Framebuffer(Framebuffer&& other) noexcept
        :Framebuffer()
    {
        *this = std::move(other);
    }

    Framebuffer::~Framebuffer()
    {
        if (m_handle == nullptr) return;
        ((DeviceHandle) *currentDevice).destroyFramebuffer(m_handle);
        m_handle = nullptr;
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