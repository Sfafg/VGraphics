#pragma once
#include "Handle.h"
#include "Flags.h"
#include "Enums.h"

namespace vg
{
    class Image
    {
    public:
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

        Image(
            uint32_t width,
            std::vector<Format> formatCandidates,
            Flags<FormatFeature> features,
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
            std::vector<Format> formatCandidates,
            Flags<FormatFeature> features,
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
            std::vector<Format> formatCandidates,
            Flags<FormatFeature> features,
            Flags<ImageUsage> usage,
            int mipLevels = 1,
            int arrayLevels = 1,
            ImageTiling tiling = ImageTiling::Optimal,
            ImageLayout initialLayout = ImageLayout::Undefined,
            int samples = 1,
            SharingMode sharingMode = SharingMode::Exclusive);

        Image();
        Image(Image&& other) noexcept;
        Image(const Image& other) = delete;
        ~Image();

        Image& operator=(Image&& other) noexcept;
        Image& operator=(const Image& other) = delete;
        operator const ImageHandle& () const;

        Format GetFormat() const;
        unsigned int GetDimensionCount() const;
        void GetDimensions(uint32_t* width, uint32_t* height, uint32_t* depth) const;
        std::vector<uint32_t> GetDimensions() const;
        uint64_t GetSize() const;
        uint64_t GetOffset() const;
        class MemoryBlock* GetMemory() const;

        /// @brief Find the supported format from candidates.
        /// @param candidates array of Formats sorted by best to worst
        /// @param tiling ImageTiling needed for the image
        /// @param features Needed features
        /// @return one of the formats that are supported or Format::Undefined
        static Format FindSupportedFormat(const std::vector<Format>& candidates, ImageTiling tiling, Flags<FormatFeature> features);

    private:
        ImageHandle m_handle;
        Format m_format;
        unsigned int m_dimensionCount;
        uint32_t m_dimensions[3];
        uint64_t m_offset;
        uint64_t m_size;

        class MemoryBlock* m_memory;

        friend class MemoryBlock;
        friend void Allocate(std::vector<Image>&, Flags<MemoryProperty>);
        friend void Allocate(std::initializer_list<Image*>, Flags<MemoryProperty>);
    };
}