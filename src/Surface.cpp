#include <vulkan/vulkan.hpp>
#include "Surface.h"
#include "Instance.h"

namespace vg
{
    Surface::Surface() :m_handle(nullptr) {}
    Surface::Surface(SurfaceHandle windowSurfaceHandle, Span<const SurfaceFormat> candidates) : m_handle(windowSurfaceHandle)
    {
        auto supportedFormats = ((PhysicalDeviceHandle) *currentDevice).getSurfaceFormatsKHR(m_handle);
        m_format = (Format) supportedFormats[0].format;
        m_colorSpace = (ColorSpace) supportedFormats[0].colorSpace;

        for (auto&& format : candidates)
        {
            for (int i = 0; i < supportedFormats.size(); i++)
            {
                if (supportedFormats[i].format == (vk::Format) format.format && supportedFormats[i].colorSpace == (vk::ColorSpaceKHR) format.colorSpace)
                {
                    m_format = format.format;
                    m_colorSpace = format.colorSpace;
                    return;
                }
            }
        }
    }

    Surface::Surface(Surface&& other) noexcept
        :Surface()
    {
        *this = std::move(other);
    }

    Surface::~Surface()
    {
        if (m_handle == nullptr) return;
        InstanceHandle(vg::instance).destroySurfaceKHR(m_handle);
        m_handle = nullptr;
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