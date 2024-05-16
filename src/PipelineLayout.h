#pragma once
#include <vector>
#include "Handle.h"
#include "Structs.h"

namespace vg
{
    class PipelineLayout
    {
    public:
        PipelineLayout();

        PipelineLayout(PipelineLayout&& other) noexcept;
        PipelineLayout(const PipelineLayout& other) = delete;

        PipelineLayout& operator=(PipelineLayout&& other) noexcept;
        PipelineLayout& operator=(const PipelineLayout& other) = delete;
        operator const PipelineLayoutHandle& () const;

        std::vector<DescriptorSetLayoutHandle>& GetDescriptorSets();
        const std::vector<DescriptorSetLayoutHandle>& GetDescriptorSets()const;

    private:
        PipelineLayoutHandle m_handle;
        std::vector<DescriptorSetLayoutHandle> m_descriptorSetLayouts;
        friend class RenderPass;
    };
};