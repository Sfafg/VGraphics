#include <vulkan/vulkan.hpp>
#include "Image.h"
#include "MemoryManager.h"
#include "CmdBuffer.h"
#include <math.h>

namespace vg {
Image::Image(
    Span<const uint32_t> extend, Format format, Flags<ImageUsage> usage, uint32_t mipLevels, int arrayLevels,
    ImageTiling tiling, ImageLayout initialLayout, int samples, SharingMode sharingMode
)
    : m_format(format), m_tiling(tiling), m_dimensionCount(extend.size()),
      m_dimensions{extend[0], (extend.size() < 2 ? 1 : extend[1]), (extend.size() < 3 ? 1 : extend[2])},
      m_mipLevels(mipLevels) {
    assert(0 < extend.size() && extend.size() < 4);

    uint32_t maximum = m_dimensions[0];
    for (int i = 1; i < m_dimensionCount; i++) maximum = std::max(m_dimensions[i], maximum);

    m_mipLevels = std::min(mipLevels, (uint32_t)std::floor(std::log2(maximum)) + 1);

    vk::ImageCreateInfo imageInfo(
        {}, (vk::ImageType)(m_dimensionCount - 1), (vk::Format)format,
        {m_dimensions[0], m_dimensions[1], m_dimensions[2]}, m_mipLevels, arrayLevels, (vk::SampleCountFlagBits)samples,
        (vk::ImageTiling)tiling, (vk::ImageUsageFlags)usage, (vk::SharingMode)sharingMode, {},
        (vk::ImageLayout)initialLayout
    );

    m_handle = ((DeviceHandle)*currentDevice).createImage(imageInfo);
}

Image::Image(
    Span<const uint32_t> extend, Span<const Format> formatCandidates, Flags<FormatFeature> features,
    Flags<ImageUsage> usage, uint32_t mipLevels, int arrayLevels, ImageTiling tiling, ImageLayout initialLayout,
    int samples, SharingMode sharingMode
)
    : Image(
          extend, FindSupportedFormat(formatCandidates, tiling, features), usage, mipLevels, arrayLevels, tiling,
          initialLayout, samples, sharingMode
      ) {}

Image::Image()
    : m_handle(nullptr), m_format(Format::Undefined), m_tiling(ImageTiling::Linear), m_dimensionCount(0),
      m_dimensions{0, 0, 0}, m_offset(0), m_size(0), m_memory(nullptr) {}

Image::Image(Image &&other) noexcept : Image() { *this = std::move(other); }
Image::~Image() {
    if (!m_handle) return;
    ((DeviceHandle)*currentDevice).destroyImage(m_handle);
    if (m_memory) m_memory->Dereferance();
    m_handle = nullptr;
}

Image &Image::operator=(Image &&other) noexcept {
    if (this == &other) return *this;

    std::swap(m_handle, other.m_handle);
    std::swap(m_memory, other.m_memory);
    std::swap(m_format, other.m_format);
    std::swap(m_tiling, other.m_tiling);
    std::swap(m_dimensionCount, other.m_dimensionCount);
    std::swap(m_dimensions, other.m_dimensions);
    std::swap(m_offset, other.m_offset);
    std::swap(m_size, other.m_size);

    return *this;
}
Image::operator const ImageHandle &() const { return m_handle; }

void Image::AppendMipmapGenerationCommands(CmdBuffer *cmdBuffer, int mipMapCount) const {
    cmd::PipelineBarier barrier(
        PipelineStage::Transfer, PipelineStage::Transfer,
        {{*this,
          ImageLayout::TransferDstOptimal,
          ImageLayout::TransferSrcOptimal,
          Access::TransferWrite,
          Access::TransferRead,
          {ImageAspect::Color, 0, 1}}}
    );
    cmd::BlitImage blitImage(
        *this, ImageLayout::TransferSrcOptimal, *this, ImageLayout::TransferDstOptimal,
        {{{ImageAspect::Color, 0}, {{0, 0, 0}, {0, 0, 0}}, {ImageAspect::Color, 0}, {{0, 0, 0}, {0, 0, 0}}}},
        Filter::Linear
    );

    int32_t mipWidth = m_dimensions[0], mipHeight = m_dimensions[1], mipDepth = m_dimensions[2];
    for (int i = 1; i < mipMapCount; i++) {
        barrier.imageMemoryBarriers[0].subresourceRange.baseMipLevel = i - 1;
        blitImage.regions[0].srcSubresource.mipLevel = i - 1;
        blitImage.regions[0].srcOffsets[1] = {mipWidth, mipHeight, mipDepth};
        blitImage.regions[0].dstSubresource.mipLevel = i;
        blitImage.regions[0].dstOffsets[1] = {
            mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, mipDepth > 1 ? mipDepth / 2 : 1
        };
        cmdBuffer->Append(barrier);
        cmdBuffer->Append(blitImage);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
        if (mipDepth > 1) mipDepth /= 2;
    }
    barrier.imageMemoryBarriers[0].subresourceRange.baseMipLevel = mipMapCount - 1;
    cmdBuffer->Append(barrier);
}

Format Image::GetFormat() const { return m_format; }

unsigned int Image::GetDimensionCount() const { return m_dimensionCount; }

void Image::GetDimensions(uint32_t *width, uint32_t *height, uint32_t *depth) const {
    *width = m_dimensions[0];
    *height = m_dimensions[1];
    *depth = m_dimensions[2];
}

Span<const uint32_t> Image::GetDimensions() const { return Span<const uint32_t>(m_dimensions, GetDimensionCount()); }

uint64_t Image::GetSize() const { return m_size; }

uint64_t Image::GetOffset() const { return m_offset; }

uint32_t Image::GetMipLevels() const { return m_mipLevels; }

class MemoryBlock *Image::GetMemory() const { return m_memory; }

Format Image::FindSupportedFormat(Span<const Format> candidates, ImageTiling tiling, Flags<FormatFeature> features) {
    for (Format format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties((PhysicalDeviceHandle)*currentDevice, (VkFormat)format, &props);
        if (tiling == ImageTiling::Linear && (props.linearTilingFeatures & features) == features) return format;
        else if (tiling == ImageTiling::Optimal && (props.optimalTilingFeatures & features) == features) return format;
    }
    return Format::Undefined;
}

} // namespace vg
