#pragma once
#include "Handle.h"

namespace vg
{
    class Surface
    {
    public:
        Surface();

        Surface(Surface&& other) noexcept;
        Surface& operator=(Surface&& other) noexcept;
        Surface(const Surface& other) = delete;
        Surface& operator=(const Surface& other) = delete;

        ~Surface();

        operator SurfaceHandle() const;

    public:
        SurfaceHandle m_handle;
    };
}