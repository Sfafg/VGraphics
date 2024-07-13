#include <vulkan/vulkan.hpp>
#include "Image.h"
#include "MemoryManager.h"

namespace vg
{
    Image::Image(
        uint32_t width,
        Format format,
        Flags<ImageUsage> usage,
        int mipLevels,
        int arrayLevels,
        ImageTiling tiling,
        ImageLayout initialLayout,
        int samples,
        SharingMode sharingMode) : m_format(format), m_dimensionCount(1), m_dimensions{ width,0,0 }
    {
        vk::ImageCreateInfo imageInfo({}, vk::ImageType::e1D,
            (vk::Format) format, { width,1,1 },
            mipLevels, arrayLevels, (vk::SampleCountFlagBits) samples,
            (vk::ImageTiling) tiling, (vk::ImageUsageFlags) usage, (vk::SharingMode) sharingMode, {}, (vk::ImageLayout) initialLayout);

        m_handle = ((DeviceHandle) currentDevice).createImage(imageInfo);
    }

    Image::Image(
        uint32_t width,
        uint32_t height,
        Format format,
        Flags<ImageUsage> usage,
        int mipLevels,
        int arrayLevels,
        ImageTiling tiling,
        ImageLayout initialLayout,
        int samples,
        SharingMode sharingMode) : m_format(format), m_dimensionCount(2), m_dimensions{ width,height,0 }
    {
        vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D,
            (vk::Format) format, { width,height,1 },
            mipLevels, arrayLevels, (vk::SampleCountFlagBits) samples,
            (vk::ImageTiling) tiling, (vk::ImageUsageFlags) usage, (vk::SharingMode) sharingMode, {}, (vk::ImageLayout) initialLayout);

        m_handle = ((DeviceHandle) currentDevice).createImage(imageInfo);
    }

    Image::Image(
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        Format format,
        Flags<ImageUsage> usage,
        int mipLevels,
        int arrayLevels,
        ImageTiling tiling,
        ImageLayout initialLayout,
        int samples,
        SharingMode sharingMode) : m_format(format), m_dimensionCount(3), m_dimensions{ width,height,depth }
    {
        vk::ImageCreateInfo imageInfo({}, vk::ImageType::e3D,
            (vk::Format) format, { width,height,depth },
            mipLevels, arrayLevels, (vk::SampleCountFlagBits) samples,
            (vk::ImageTiling) tiling, (vk::ImageUsageFlags) usage, (vk::SharingMode) sharingMode, {}, (vk::ImageLayout) initialLayout);

        m_handle = ((DeviceHandle) currentDevice).createImage(imageInfo);
    }

    Image::Image(
        uint32_t width,
        std::vector<Format> formatCandidates,
        Flags<FormatFeature> features,
        Flags<ImageUsage> usage,
        int mipLevels,
        int arrayLevels,
        ImageTiling tiling,
        ImageLayout initialLayout,
        int samples,
        SharingMode sharingMode)
        : Image(width, FindSupportedFormat(formatCandidates, tiling, features), usage, mipLevels, arrayLevels, tiling, initialLayout, samples, sharingMode)
    {}

    Image::Image(
        uint32_t width,
        uint32_t height,
        std::vector<Format> formatCandidates,
        Flags<FormatFeature> features,
        Flags<ImageUsage> usage,
        int mipLevels,
        int arrayLevels,
        ImageTiling tiling,
        ImageLayout initialLayout,
        int samples,
        SharingMode sharingMode)
        : Image(width, height, FindSupportedFormat(formatCandidates, tiling, features), usage, mipLevels, arrayLevels, tiling, initialLayout, samples, sharingMode)
    {}

    Image::Image(
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        std::vector<Format> formatCandidates,
        Flags<FormatFeature> features,
        Flags<ImageUsage> usage,
        int mipLevels,
        int arrayLevels,
        ImageTiling tiling,
        ImageLayout initialLayout,
        int samples,
        SharingMode sharingMode)
        : Image(width, height, depth, FindSupportedFormat(formatCandidates, tiling, features), usage, mipLevels, arrayLevels, tiling, initialLayout, samples, sharingMode)
    {}

    Image::Image() :m_handle(nullptr), m_format(Format::Undefined), m_dimensionCount(0), m_dimensions{ 0,0,0 }, m_offset(0), m_size(0), m_memory(nullptr) {}

    Image::Image(Image&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_memory, other.m_memory);
    }
    Image::~Image()
    {
        if (!m_handle) return;
        ((DeviceHandle) currentDevice).destroyImage(m_handle);
        if (m_memory) m_memory->Dereferance();
    }

    Image& Image::operator=(Image&& other) noexcept
    {
        if (this == &other) return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_memory, other.m_memory);

        return *this;
    }
    Image::operator const ImageHandle& () const
    {
        return m_handle;
    }

    Format Image::GetFormat() const
    {
        return m_format;
    }

    unsigned int Image::GetDimensionCount() const
    {
        return m_dimensionCount;
    }

    void Image::GetDimensions(uint32_t* width, uint32_t* height, uint32_t* depth) const
    {
        *width = m_dimensions[0];
        *height = m_dimensions[1];
        *depth = m_dimensions[2];
    }

    std::vector<uint32_t> Image::GetDimensions() const
    {
        std::vector<uint32_t> dimensions(GetDimensionCount());
        for (int i = 0; i < dimensions.size(); i++)
            dimensions.push_back(m_dimensions[i]);

        return dimensions;
    }

    uint64_t Image::GetSize() const
    {
        return m_size;
    }

    uint64_t Image::GetOffset() const
    {
        return m_offset;
    }

    class MemoryBlock* Image::GetMemory() const
    {
        return m_memory;
    }

    Format Image::FindSupportedFormat(const std::vector<Format>& candidates, ImageTiling tiling, Flags<FormatFeature> features)
    {
        for (Format format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties((PhysicalDeviceHandle) currentDevice, (VkFormat) format, &props);
            if (tiling == ImageTiling::Linear && (props.linearTilingFeatures & features) == features)
                return format;
            else if (tiling == ImageTiling::Optimal && (props.optimalTilingFeatures & features) == features)
                return format;
        }
        return Format::Undefined;
    }

}