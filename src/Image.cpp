#include <vulkan/vulkan.hpp>
#include "Image.h"
#include "MemoryManager.h"

namespace vg
{
    Image::Image() :m_handle(nullptr), m_memory(nullptr) {}

    Image::Image(
        uint32_t width,
        Format format,
        Flags<ImageUsage> usage,
        int mipLevels,
        int arrayLevels,
        ImageTiling tiling,
        ImageLayout initialLayut,
        int samples,
        SharingMode sharingMode)
    {
        vk::ImageCreateInfo imageInfo({}, vk::ImageType::e1D,
            (vk::Format) format, { width,1,1 },
            mipLevels, arrayLevels, (vk::SampleCountFlagBits) samples,
            (vk::ImageTiling) tiling, (vk::ImageUsageFlags) usage, (vk::SharingMode) sharingMode);

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
        ImageLayout initialLayut,
        int samples,
        SharingMode sharingMode)
    {
        vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D,
            (vk::Format) format, { width,height,1 },
            mipLevels, arrayLevels, (vk::SampleCountFlagBits) samples,
            (vk::ImageTiling) tiling, (vk::ImageUsageFlags) usage, (vk::SharingMode) sharingMode);

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
        ImageLayout initialLayut,
        int samples,
        SharingMode sharingMode)
    {
        vk::ImageCreateInfo imageInfo({}, vk::ImageType::e3D,
            (vk::Format) format, { width,height,depth },
            mipLevels, arrayLevels, (vk::SampleCountFlagBits) samples,
            (vk::ImageTiling) tiling, (vk::ImageUsageFlags) usage, (vk::SharingMode) sharingMode);

        m_handle = ((DeviceHandle) currentDevice).createImage(imageInfo);
    }

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
}