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

        void SetBufferInfo(Buffer buffer, uint64_t offset, uint64_t range, unsigned int dstBinding, unsigned int dstArrayElement, DescriptorType type);

    private:
        DescriptorSetHandle m_handle;
    };
}