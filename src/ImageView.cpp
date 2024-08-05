#include <vulkan/vulkan.hpp>
#include "Device.h"
#include "ImageView.h"

namespace vg
{
    ImageView::ImageView(const Image& image, ImageSubresource subresourceRange) :ImageView(image, (ImageViewType) (image.GetDimensionCount() - 1), image.GetFormat(), subresourceRange)
    {}

    ImageView::ImageView(const Image& image, ImageViewType type, Format format, ImageSubresource subresourceRange)
    {
        vk::ImageViewCreateInfo createInfo({}, (ImageHandle) image, (vk::ImageViewType) type, (vk::Format) format, vk::ComponentMapping(), subresourceRange);

        m_handle = ((DeviceHandle) *currentDevice).createImageView(createInfo);
    }

    ImageView::ImageView() : m_handle(nullptr) {}

    ImageView::ImageView(ImageView&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
    }


    ImageView::~ImageView()
    {
        if (!m_handle) return;
        ((DeviceHandle) *currentDevice).destroyImageView(m_handle);
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