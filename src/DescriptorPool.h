#pragma once
#include "Handle.h"
#include "Device.h"
#include "Enums.h"
#include "Structs.h"
#include "DescriptorSet.h"

namespace vg
{
    class DescriptorPool
    {
    public:
        DescriptorPool(unsigned int maxSets, std::initializer_list<DescriptorPoolSize> sizes);

        DescriptorPool();
        DescriptorPool(DescriptorPool&& other) noexcept;
        DescriptorPool(const DescriptorPool& other) = delete;
        ~DescriptorPool();

        DescriptorPool& operator=(DescriptorPool&& other) noexcept;
        DescriptorPool& operator=(const DescriptorPool& other) = delete;
        operator const DescriptorPoolHandle& () const;

        std::vector<DescriptorSet> Allocate(const std::vector<DescriptorSetLayoutHandle>& setLayouts);

    private:
        DescriptorPoolHandle m_handle;

    };
}