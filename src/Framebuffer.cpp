#include <vulkan/vulkan.hpp>
#include "Framebuffer.h"

namespace vg
{
    Framebuffer::Framebuffer() :m_handle(nullptr), m_device(nullptr) {}

    Framebuffer::Framebuffer(DeviceHandle device, RenderPassHandle renderPass, const std::vector<ImageViewHandle>& attachments, unsigned int width, unsigned int height, unsigned int layers) : m_device(device)
    {
        m_handle = vk::Device((vg::DeviceHandle) m_device).createFramebuffer({ {}, renderPass, *(std::vector<vk::ImageView>*) & attachments, width, height, layers });
    }

    Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);
        other.m_handle = nullptr;
    }

    Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);
        other.m_handle = nullptr;

        return *this;
    }

    Framebuffer::~Framebuffer()
    {
        if (m_handle == nullptr) return;
        vk::Device((vg::DeviceHandle) m_device).destroyFramebuffer(m_handle);
    }

    Framebuffer::operator FramebufferHandle() const
    {
        return m_handle;
    }
}