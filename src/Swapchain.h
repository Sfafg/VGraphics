#pragma once
#include <vector>
#include "Device.h"
#include "Surface.h"
#include "Enums.h"
#include "MySemaphore.h"
#include "Fence.h"

namespace vg
{
    class Swapchain
    {
    public:
        Swapchain(const Surface& surface, const Device& device, unsigned int imageCount, unsigned int width, unsigned int height, Usage usage = Usage::ColorAttachment, PresentMode presentMode = PresentMode::Fifo, CompositeAlpha alpha = CompositeAlpha::Opaque, SwapchainHandle oldSwapchain = SwapchainHandle());

        Swapchain();
        Swapchain(Swapchain&& other) noexcept;
        Swapchain(const Swapchain& other) = delete;
        ~Swapchain();

        Swapchain& operator=(Swapchain&& other) noexcept;
        Swapchain& operator=(const Swapchain& other) = delete;

        operator const SwapchainHandle& () const;

        unsigned int GetWidth() const;
        unsigned int GetHeight() const;
        const std::vector<ImageViewHandle>& GetImageViews() const;
        uint32_t GetNextImageIndex(uint64_t timeout, const Semaphore& semaphore, const  Fence& fence);

    private:
        SwapchainHandle m_handle;
        DeviceHandle m_device;
        std::vector<ImageHandle> m_images;
        std::vector<ImageViewHandle> m_imageViews;

        unsigned int m_width, m_height;
    };
}