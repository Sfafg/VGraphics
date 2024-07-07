#pragma once
#include "Handle.h"
#include "Flags.h"
#include "Enums.h"
#include "Structs.h"
#include "Image.h"

namespace vg
{
    class ImageView
    {
    public:
        ImageView();

        ImageView(const Image& image, ImageViewType type, Format format, ImageSubresourceRange subresourceRange);

        ImageView(ImageView&& other) noexcept;
        ImageView(const ImageView& other) = delete;
        ~ImageView();

        ImageView& operator=(ImageView&& other) noexcept;
        ImageView& operator=(const ImageView& other) = delete;
        operator const ImageViewHandle& () const;

    private:
        ImageViewHandle m_handle;
    };
}