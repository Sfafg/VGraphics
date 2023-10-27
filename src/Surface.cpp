#include <vulkan/vulkan.hpp>
#include "Surface.h"
#include "Instance.h"

namespace vg
{
    Surface::Surface() :m_handle(nullptr) {}

    Surface::Surface(Surface&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        other.m_handle = nullptr;
    }

    Surface& Surface::operator=(Surface&& other) noexcept
    {
        if (&other == this) return *this;
        std::swap(m_handle, other.m_handle);
        other.m_handle = nullptr;

        return *this;
    }

    Surface::~Surface()
    {
        if (m_handle == nullptr) return;
        vk::Instance((InstanceHandle) instance).destroySurfaceKHR(m_handle);
    }

    Surface::operator SurfaceHandle() const
    {
        return m_handle;
    }
}