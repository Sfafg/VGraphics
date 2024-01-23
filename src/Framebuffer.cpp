#include <vulkan/vulkan.hpp>
#include "Framebuffer.h"

namespace vg
{
    Framebuffer::Framebuffer(DeviceHandle device, RenderPassHandle renderPass, const std::vector<ImageViewHandle>& attachments, unsigned int width, unsigned int height, unsigned int layers) : m_device(device)
    {
        m_handle = m_device.createFramebuffer({ {}, renderPass, *(std::vector<vk::ImageView>*) & attachments, width, height, layers });
    }

    Framebuffer::Framebuffer() :m_handle(nullptr), m_device(nullptr) {}

    Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);
    }

    Framebuffer::~Framebuffer()
    {
        if (m_handle == nullptr) return;
        m_device.destroyFramebuffer(m_handle);
    }

    Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);

        return *this;
    }

    Framebuffer::operator const FramebufferHandle& () const
    {
        return m_handle;
    }
}