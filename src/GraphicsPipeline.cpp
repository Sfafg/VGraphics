#include <vulkan/vulkan.hpp>
#include "GraphicsPipeline.h"
#include "Device.h"
#include <iostream>

namespace vg
{
    GraphicsPipeline::GraphicsPipeline(
        const std::vector<DescriptorSetLayoutBinding>& setLayoutBindings,
        const std::vector<PushConstantRange>& pushConstantRanges,
        const std::vector<Shader*>& shaders,
        const VertexLayout& vertexInput,
        const InputAssembly& inputAssembly,
        const Tesselation& tesselation,
        const ViewportState& viewportState,
        const Rasterizer& rasterizer,
        const Multisampling& multisampling,
        const DepthStencil& depthStencil,
        const ColorBlending& colorBlending,
        const std::vector<DynamicState>& dynamicState,
        uint32_t childrenCount
    ) :
        m_createInfo(new CreateInfo(
            childrenCount, setLayoutBindings, pushConstantRanges, shaders, vertexInput,
            inputAssembly, tesselation, viewportState, rasterizer,
            multisampling, depthStencil, colorBlending, dynamicState, -1, nullptr
        )), m_handle(nullptr)
    {}


    GraphicsPipeline::GraphicsPipeline(
        uint32_t parentIndex,
        const std::optional<std::vector<DescriptorSetLayoutBinding>>& setLayoutBindings,
        const std::optional<std::vector<PushConstantRange>>& pushConstantRanges,
        const std::optional<std::vector<Shader*>>& shaders,
        const std::optional<VertexLayout>& vertexInput,
        const std::optional<InputAssembly>& inputAssembly,
        const std::optional<Tesselation>& tesselation,
        const std::optional<ViewportState>& viewportState,
        const std::optional<Rasterizer>& rasterizer,
        const std::optional<Multisampling>& multisampling,
        const std::optional<DepthStencil>& depthStencil,
        const std::optional<ColorBlending>& colorBlending,
        const std::optional<std::vector<DynamicState>>& dynamicState,
        uint32_t childrenCount
    ) :
        m_createInfo(new CreateInfo(
            childrenCount, setLayoutBindings, pushConstantRanges, shaders, vertexInput,
            inputAssembly, tesselation, viewportState, rasterizer,
            multisampling, depthStencil, colorBlending, dynamicState, parentIndex, nullptr
        )), m_handle(nullptr)
    {}

    GraphicsPipeline::GraphicsPipeline(
        GraphicsPipeline* parent,
        const std::optional<std::vector<DescriptorSetLayoutBinding>>& setLayoutBindings,
        const std::optional<std::vector<PushConstantRange>>& pushConstantRanges,
        const std::optional<std::vector<Shader*>>& shaders,
        const std::optional<VertexLayout>& vertexInput,
        const std::optional<InputAssembly>& inputAssembly,
        const std::optional<Tesselation>& tesselation,
        const std::optional<ViewportState>& viewportState,
        const std::optional<Rasterizer>& rasterizer,
        const std::optional<Multisampling>& multisampling,
        const std::optional<DepthStencil>& depthStencil,
        const std::optional<ColorBlending>& colorBlending,
        const std::optional<std::vector<DynamicState>>& dynamicState,
        uint32_t childrenCount
    ) :
        m_createInfo(new CreateInfo(
            childrenCount, setLayoutBindings, pushConstantRanges, shaders, vertexInput,
            inputAssembly, tesselation, viewportState, rasterizer,
            multisampling, depthStencil, colorBlending, dynamicState, -1, parent
        )), m_handle(nullptr)
    {}

    GraphicsPipeline::GraphicsPipeline() :m_handle(nullptr), m_createInfo(nullptr) {}

    GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
        std::swap(m_createInfo, other.m_createInfo);
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
        if (m_handle == nullptr)
            return;

        ((DeviceHandle) *currentDevice).destroyPipeline(m_handle);
        m_handle = nullptr;

#ifdef VG_DEBUG
        if (m_createInfo && m_createInfo->inheritanceCount > 0)
        {
            std::cout << "Not all pipeline children used, please use all children or lower the children count, Count: " << m_createInfo->inheritanceCount << '\n';
        }
#elif VG_NDEBUG
#elif NDEBUG
#else
        if (m_createInfo && m_createInfo->inheritanceCount > 0)
        {
            std::cout << "Not all pipeline children used, please use all children or lower the children count, Count: " << m_createInfo->inheritanceCount << '\n';
        }
#endif
    }

    GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept
    {
        if (&other == this)
            return *this;

        std::swap(m_handle, other.m_handle);
        std::swap(m_createInfo, other.m_createInfo);

        return *this;
    }

    GraphicsPipeline::operator const GraphicsPipelineHandle& () const
    {
        return m_handle;
    }

    std::vector<PushConstantRange>* GraphicsPipeline::CreateInfo::GetPushConstantRanges()
    {
        if (pushConstantRanges)
            return &*pushConstantRanges;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetPushConstantRanges();

        return nullptr;
    }

    std::vector<DescriptorSetLayoutBinding>* GraphicsPipeline::CreateInfo::GetSetLayoutBindings()
    {
        if (setLayoutBindings)
            return &*setLayoutBindings;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetSetLayoutBindings();

        return nullptr;
    }

    std::vector<Shader*>* GraphicsPipeline::CreateInfo::GetShaders()
    {
        if (shaders)
            return &*shaders;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetShaders();

        return nullptr;
    }

    VertexLayout* GraphicsPipeline::CreateInfo::GetVertexInput()
    {
        if (vertexInput)
            return &*vertexInput;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetVertexInput();

        return nullptr;
    }

    InputAssembly* GraphicsPipeline::CreateInfo::GetInputAssembly()
    {
        if (inputAssembly)
            return &*inputAssembly;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetInputAssembly();

        return nullptr;
    }

    Tesselation* GraphicsPipeline::CreateInfo::GetTesselation()
    {
        if (tesselation)
            return &*tesselation;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetTesselation();

        return nullptr;
    }

    ViewportState* GraphicsPipeline::CreateInfo::GetViewportState()
    {
        if (viewportState)
            return &*viewportState;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetViewportState();

        return nullptr;
    }

    Rasterizer* GraphicsPipeline::CreateInfo::GetRasterizer()
    {
        if (rasterizer)
            return &*rasterizer;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetRasterizer();

        return nullptr;
    }

    Multisampling* GraphicsPipeline::CreateInfo::GetMultisampling()
    {
        if (multisampling)
            return &*multisampling;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetMultisampling();

        return nullptr;
    }

    DepthStencil* GraphicsPipeline::CreateInfo::GetDepthStencil()
    {
        if (depthStencil)
            return &*depthStencil;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetDepthStencil();

        return nullptr;
    }

    ColorBlending* GraphicsPipeline::CreateInfo::GetColorBlending()
    {
        if (colorBlending)
            return &*colorBlending;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetColorBlending();

        return nullptr;
    }

    std::vector<DynamicState>* GraphicsPipeline::CreateInfo::GetDynamicState()
    {
        if (dynamicState)
            return &*dynamicState;

        if (parent && parent->m_createInfo)
            return parent->m_createInfo->GetDynamicState();

        return nullptr;
    }

    GraphicsPipelineHandle GraphicsPipeline::CreateInfo::GetParentHandle()
    {
        if (parent)
            return parent->m_handle;

        return GraphicsPipelineHandle(nullptr);
    }

    void GraphicsPipeline::CreateInfo::DecrementParentInheritance()
    {
        if (parent && parent->m_createInfo)
        {
#ifdef VG_DEBUG
            if (parent->m_createInfo->inheritanceCount == 0)
                std::cout << "Trying to inherit from a pipeline, but children count is full.\n";
#elif VG_NDEBUG
#elif NDEBUG
#else
            if (parent->m_createInfo->inheritanceCount == 0)
                std::cout << "Trying to inherit from a pipeline, but children count is full.\n";
#endif
            parent->m_createInfo->inheritanceCount--;
            parent->m_createInfo->DecrementParentInheritance();
        }
    }

    void GraphicsPipeline::CreateInfo::UpdateParentInheritance()
    {
        if (parent && parent->m_createInfo)
        {
            if (parent->m_createInfo->inheritanceCount == 0)
            {
                parent->m_createInfo->UpdateParentInheritance();
                delete parent->m_createInfo;
                parent->m_createInfo = nullptr;
            }
        }
    }
}