#include <vulkan/vulkan.hpp>
#include "Device.h"
#include "ImageView.h"

namespace vg
{
    ImageView::ImageView() : m_handle(nullptr) {}

    ImageView::ImageView(const Image& image, ImageViewType type, Format format, ImageSubresourceRange subresourceRange)
    {
        vk::ImageViewCreateInfo createInfo({}, (ImageHandle) image, (vk::ImageViewType) type, (vk::Format) format, vk::ComponentMapping(), subresourceRange);

        m_handle = ((DeviceHandle) currentDevice).createImageView(createInfo);
    }

    ImageView::ImageView(ImageView&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
    }


    ImageView::~ImageView()
    {
        if (!m_handle) return;
        ((DeviceHandle) currentDevice).destroyImageView(m_handle);
    }

    ImageView& ImageView::operator=(ImageView&& other) noexcept
    {
        if (this == &other) return *this;

        std::swap(m_handle, other.m_handle);

        return *this;
    }

    ImageView::operator const ImageViewHandle& () const
    {
        return m_handle;
    }
}