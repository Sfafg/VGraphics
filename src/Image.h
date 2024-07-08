#pragma once
#include "Handle.h"
#include "Flags.h"
#include "Enums.h"

namespace vg
{
    class Image
    {
    public:
        Image();

        Image(
            uint32_t width,
            Format format,
            Flags<ImageUsage> usage,
            int mipLevels = 1,
            int arrayLevels = 1,
            ImageTiling tiling = ImageTiling::Optimal,
            ImageLayout initialLayout = ImageLayout::Undefined,
            int samples = 1,
            SharingMode sharingMode = SharingMode::Exclusive);

        Image(
            uint32_t width,
            uint32_t height,
            Format format,
            Flags<ImageUsage> usage,
            int mipLevels = 1,
            int arrayLevels = 1,
            ImageTiling tiling = ImageTiling::Optimal,
            ImageLayout initialLayout = ImageLayout::Undefined,
            int samples = 1,
            SharingMode sharingMode = SharingMode::Exclusive);

        Image(
            uint32_t width,
            uint32_t height,
            uint32_t depth,
            Format format,
            Flags<ImageUsage> usage,
            int mipLevels = 1,
            int arrayLevels = 1,
            ImageTiling tiling = ImageTiling::Optimal,
            ImageLayout initialLayout = ImageLayout::Undefined,
            int samples = 1,
            SharingMode sharingMode = SharingMode::Exclusive);

        Image(Image&& other) noexcept;
        Image(const Image& other) = delete;
        ~Image();

        Image& operator=(Image&& other) noexcept;
        Image& operator=(const Image& other) = delete;
        operator const ImageHandle& () const;

        uint64_t GetSize() const;
        uint64_t GetOffset() const;
        class MemoryBlock* GetMemory() const;

    private:
        ImageHandle m_handle;
        uint64_t m_offset;
        uint64_t m_size;

        class MemoryBlock* m_memory;

        friend class MemoryBlock;
        friend void Allocate(std::vector<Image>&, Flags<MemoryProperty>);
        friend void Allocate(std::initializer_list<Image*>, Flags<MemoryProperty>);
    };
}