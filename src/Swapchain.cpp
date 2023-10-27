#include <vulkan/vulkan.hpp>
#include "Swapchain.h"

namespace vg
{
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::set<vk::SurfaceFormatKHR> formats;
        std::set<vk::PresentModeKHR> presentModes;

        SwapChainSupportDetails(vk::PhysicalDevice device, vk::SurfaceKHR surface)
        {
            capabilities = device.getSurfaceCapabilitiesKHR(surface);
            for (const auto& format : device.getSurfaceFormatsKHR(surface)) formats.insert(format);
            for (const auto& presentMode : device.getSurfacePresentModesKHR(surface)) presentModes.insert(presentMode);
        }
    };

    Swapchain::Swapchain() : m_handle(nullptr), m_device(nullptr) {}

    Swapchain::Swapchain(SurfaceHandle surface, const Device& device, unsigned int imageCount, unsigned int width, unsigned int height, Usage usage, PresentMode presentMode, CompositeAlpha alpha) : m_device(device)
    {
        // Get the sharing mode, needed if queues are in different families.
        vk::SharingMode sharingMode = vk::SharingMode::eExclusive;
        std::vector<unsigned int> indices;
        if (device.graphicsQueue.GetIndex() != device.presentQueue.GetIndex())
        {
            indices.assign({ device.graphicsQueue.GetIndex(), device.presentQueue.GetIndex() });
            sharingMode = vk::SharingMode::eConcurrent;
        }

        // Check if parameters for Swapchain creation are valid and change them if not.
        SwapChainSupportDetails supportDetails((PhysicalDeviceHandle) device, surface);
        imageCount = std::clamp(imageCount, supportDetails.capabilities.minImageCount, supportDetails.capabilities.maxImageCount);

        m_format = (Format) vk::Format::eB8G8R8A8Srgb;
        vk::ColorSpaceKHR colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        if (!supportDetails.formats.contains({ (vk::Format) m_format, colorSpace }))
        {
            m_format = (Format) supportDetails.formats.begin()->format;
            colorSpace = supportDetails.formats.begin()->colorSpace;
        }

        if (supportDetails.capabilities.currentExtent.width != UINT_MAX)
        {
            m_width = supportDetails.capabilities.currentExtent.width;
            m_height = supportDetails.capabilities.currentExtent.height;
        }
        else
        {
            vk::Extent2D min = supportDetails.capabilities.minImageExtent;
            vk::Extent2D max = supportDetails.capabilities.maxImageExtent;
            m_width = std::clamp(width, min.width, max.width);
            m_height = std::clamp(height, min.height, max.height);
        }

        if (!supportDetails.presentModes.contains((vk::PresentModeKHR) presentMode))
        {
            presentMode = PresentMode::Fifo;
        }

        // Create Swapchain and get its Images and ImageViews.
        vk::SwapchainCreateInfoKHR createInfo(
            {}, surface, imageCount, (vk::Format) m_format, colorSpace, { m_width, m_height }, 1, (vk::ImageUsageFlagBits) usage,
            sharingMode, indices, supportDetails.capabilities.currentTransform, (vk::CompositeAlphaFlagBitsKHR) alpha, (vk::PresentModeKHR) presentMode, 1, nullptr
        );

        m_handle = vk::Device((DeviceHandle) device).createSwapchainKHR(createInfo);
        for (const auto& image : vk::Device((DeviceHandle) device).getSwapchainImagesKHR(m_handle))
            m_images.push_back(image);

        m_imageViews.resize(m_images.size());
        for (unsigned int i = 0; i < m_images.size(); i++)
        {
            vk::ComponentMapping componentMapping;
            vk::ImageSubresourceRange subresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            vk::ImageViewCreateInfo createInfo({}, m_images[i], vk::ImageViewType::e2D, (vk::Format) m_format, {}, subresourceRange);

            m_imageViews[i] = vk::Device((DeviceHandle) device).createImageView(createInfo);
        }
    }

    Swapchain::Swapchain(Swapchain&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);
        std::swap(m_images, other.m_images);
        other.m_handle = nullptr;
    }

    Swapchain& Swapchain::operator=(Swapchain&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_device, other.m_device);
        std::swap(m_images, other.m_images);
        other.m_handle = nullptr;

        return *this;
    }

    Swapchain::~Swapchain()
    {
        if (m_handle == nullptr) return;
        for (auto imageView : m_imageViews)
        {
            vk::Device(m_device).destroyImageView(imageView);
        }
        vk::Device(m_device).destroySwapchainKHR(m_handle);
    }

    unsigned int Swapchain::GetWidth() const
    {
        return m_width;
    }

    unsigned int Swapchain::GetHeight() const
    {
        return m_height;
    }
    Format Swapchain::GetFormat() const
    {
        return m_format;
    }

    const std::vector<ImageViewHandle>& Swapchain::GetImageViews() const
    {
        return m_imageViews;
    }

    Swapchain::operator SwapchainHandle() const
    {
        return m_handle;
    }
}