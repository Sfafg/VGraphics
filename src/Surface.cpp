#include <vulkan/vulkan.hpp>
#include "Surface.h"
#include "Instance.h"

namespace vg
{
    Surface::Surface() :m_handle(nullptr) {}
    Surface::Surface(SurfaceHandle windowSurfaceHandle, Format hintedFormat, ColorSpace hintedColorSpace) : m_handle(windowSurfaceHandle)
    {
        auto supportedFormats = ((PhysicalDeviceHandle) currentDevice).getSurfaceFormatsKHR(m_handle);
        m_format = (Format) supportedFormats[0].format;
        m_colorSpace = (ColorSpace) supportedFormats[0].colorSpace;

        for (int i = 0; i < supportedFormats.size(); i++)
        {
            if (supportedFormats[i].format == (vk::Format) hintedFormat && supportedFormats[i].colorSpace == (vk::ColorSpaceKHR) hintedColorSpace)
            {
                m_format = hintedFormat;
                m_colorSpace = hintedColorSpace;
                break;
            }
        }
    }

    Surface::Surface(Surface&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_format, other.m_format);
        std::swap(m_colorSpace, other.m_colorSpace);
    }

    Surface::~Surface()
    {
        if (m_handle == nullptr) return;
        InstanceHandle(vg::instance).destroySurfaceKHR(m_handle);
    }

    Surface& Surface::operator=(Surface&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        std::swap(m_format, other.m_format);
        std::swap(m_colorSpace, other.m_colorSpace);

        return *this;
    }

    Surface::operator const SurfaceHandle& () const
    {
        return m_handle;
    }

    Surface::operator SurfaceHandle& ()
    {
        return m_handle;
    }

    Format Surface::GetFormat() const
    {
        return m_format;
    }

    ColorSpace Surface::GetColorSpace() const
    {
        return m_colorSpace;
    }
}