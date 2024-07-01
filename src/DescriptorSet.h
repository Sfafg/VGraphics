#pragma once
#include "Handle.h"
#include "Device.h"
#include "Enums.h"
#include "Structs.h"
#include "Buffer.h"

namespace vg
{
    class DescriptorSet
    {
    public:
        operator const DescriptorSetHandle& () const;

        void AttachBuffer(const Buffer& buffer, int offset, int range, int binding, int arrayElement);
    private:
        DescriptorSetHandle m_handle;
    };
}