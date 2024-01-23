#pragma once
#include "Handle.h"
#include "Enums.h"
#include "Device.h"

namespace vg
{
    class Surface
    {
    public:
        Surface();
        Surface(const Device& device, SurfaceHandle windowSurfaceHandle, Format hintedFormat, ColorSpace hintedColorSpace);
        Surface(Surface&& other) noexcept;
        Surface(const Surface& other) = delete;
        ~Surface();

        Surface& operator=(Surface&& other) noexcept;
        Surface& operator=(const Surface& other) = delete;
        operator const SurfaceHandle& () const;
        operator SurfaceHandle& ();

        Format GetFormat() const;
        ColorSpace GetColorSpace() const;

    private:
        SurfaceHandle m_handle;
        Format m_format;
        ColorSpace m_colorSpace;
    };
}