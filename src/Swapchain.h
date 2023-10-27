#pragma once
#include <vector>
#include "Device.h"
#include "VulkanEnums.h"

namespace vg
{
    class Swapchain
    {
    public:
        Swapchain();
        Swapchain(SurfaceHandle surface, const Device& device, unsigned int imageCount, unsigned int width, unsigned int height, Usage usage = Usage::ColorAttachment, PresentMode presentMode = PresentMode::Fifo, CompositeAlpha alpha = CompositeAlpha::Opaque);

        Swapchain(Swapchain&& other) noexcept;
        Swapchain& operator=(Swapchain&& other) noexcept;
        Swapchain(const Swapchain& other) = delete;
        Swapchain& operator=(const Swapchain& other) = delete;

        ~Swapchain();

        unsigned int GetWidth() const;
        unsigned int GetHeight() const;
        Format GetFormat() const;
        const std::vector<ImageViewHandle>& GetImageViews() const;
        operator SwapchainHandle() const;


    private:
        SwapchainHandle m_handle;
        DeviceHandle m_device;
        std::vector<ImageHandle> m_images;
        std::vector<ImageViewHandle> m_imageViews;

        unsigned int m_width, m_height;
        Format m_format;
    };
}