#pragma once
#include <cstring>
#include <cstdint>
#include "Enums.h"
#include "Flags.h"
#include "Queue.h"

#define VULKAN_NATIVE_CAST_OPERATOR(nativeName)\
operator const vk::nativeName& () const { return *reinterpret_cast<const vk::nativeName*>(this); }\
operator const vk::nativeName& () { return *reinterpret_cast<vk::nativeName*>(this); }

namespace vg
{
    struct DeviceLimits
    {
        uint32_t maxImageDimension1D;
        uint32_t maxImageDimension2D;
        uint32_t maxImageDimension3D;
        uint32_t maxImageDimensionCube;
        uint32_t maxImageArrayLayers;
        uint32_t maxTexelBufferElements;
        uint32_t maxUniformBufferRange;
        uint32_t maxStorageBufferRange;
        uint32_t maxPushConstantsSize;
        uint32_t maxMemoryAllocationCount;
        uint32_t maxSamplerAllocationCount;
        uint64_t bufferImageGranularity;
        uint64_t sparseAddressSpaceSize;
        uint32_t maxBoundDescriptorSets;
        uint32_t maxPerStageDescriptorSamplers;
        uint32_t maxPerStageDescriptorUniformBuffers;
        uint32_t maxPerStageDescriptorStorageBuffers;
        uint32_t maxPerStageDescriptorSampledImages;
        uint32_t maxPerStageDescriptorStorageImages;
        uint32_t maxPerStageDescriptorInputAttachments;
        uint32_t maxPerStageResources;
        uint32_t maxDescriptorSetSamplers;
        uint32_t maxDescriptorSetUniformBuffers;
        uint32_t maxDescriptorSetUniformBuffersDynamic;
        uint32_t maxDescriptorSetStorageBuffers;
        uint32_t maxDescriptorSetStorageBuffersDynamic;
        uint32_t maxDescriptorSetSampledImages;
        uint32_t maxDescriptorSetStorageImages;
        uint32_t maxDescriptorSetInputAttachments;
        uint32_t maxVertexInputAttributes;
        uint32_t maxVertexInputBindings;
        uint32_t maxVertexInputAttributeOffset;
        uint32_t maxVertexInputBindingStride;
        uint32_t maxVertexOutputComponents;
        uint32_t maxTessellationGenerationLevel;
        uint32_t maxTessellationPatchSize;
        uint32_t maxTessellationControlPerVertexInputComponents;
        uint32_t maxTessellationControlPerVertexOutputComponents;
        uint32_t maxTessellationControlPerPatchOutputComponents;
        uint32_t maxTessellationControlTotalOutputComponents;
        uint32_t maxTessellationEvaluationInputComponents;
        uint32_t maxTessellationEvaluationOutputComponents;
        uint32_t maxGeometryShaderInvocations;
        uint32_t maxGeometryInputComponents;
        uint32_t maxGeometryOutputComponents;
        uint32_t maxGeometryOutputVertices;
        uint32_t maxGeometryTotalOutputComponents;
        uint32_t maxFragmentInputComponents;
        uint32_t maxFragmentOutputAttachments;
        uint32_t maxFragmentDualSrcAttachments;
        uint32_t maxFragmentCombinedOutputResources;
        uint32_t maxComputeSharedMemorySize;
        uint32_t maxComputeWorkGroupCount[3];
        uint32_t maxComputeWorkGroupInvocations;
        uint32_t maxComputeWorkGroupSize[3];
        uint32_t subPixelPrecisionBits;
        uint32_t subTexelPrecisionBits;
        uint32_t mipmapPrecisionBits;
        uint32_t maxDrawIndexedIndexValue;
        uint32_t maxDrawIndirectCount;
        float maxSamplerLodBias;
        float maxSamplerAnisotropy;
        uint32_t maxViewports;
        uint32_t maxViewportDimensions[2];
        float viewportBoundsRange[2];
        uint32_t viewportSubPixelBits;
        size_t minMemoryMapAlignment;
        uint64_t minTexelBufferOffsetAlignment;
        uint64_t minUniformBufferOffsetAlignment;
        uint64_t minStorageBufferOffsetAlignment;
        int32_t minTexelOffset;
        uint32_t maxTexelOffset;
        int32_t minTexelGatherOffset;
        uint32_t maxTexelGatherOffset;
        float minInterpolationOffset;
        float maxInterpolationOffset;
        uint32_t subPixelInterpolationOffsetBits;
        uint32_t maxFramebufferWidth;
        uint32_t maxFramebufferHeight;
        uint32_t maxFramebufferLayers;
        uint32_t framebufferColorSampleCounts;
        uint32_t framebufferDepthSampleCounts;
        uint32_t framebufferStencilSampleCounts;
        uint32_t framebufferNoAttachmentsSampleCounts;
        uint32_t maxColorAttachments;
        uint32_t sampledImageColorSampleCounts;
        uint32_t sampledImageIntegerSampleCounts;
        uint32_t sampledImageDepthSampleCounts;
        uint32_t sampledImageStencilSampleCounts;
        uint32_t storageImageSampleCounts;
        uint32_t maxSampleMaskWords;
        uint32_t timestampComputeAndGraphics;
        float timestampPeriod;
        uint32_t maxClipDistances;
        uint32_t maxCullDistances;
        uint32_t maxCombinedClipAndCullDistances;
        uint32_t discreteQueuePriorities;
        float pointSizeRange[2];
        float lineWidthRange[2];
        float pointSizeGranularity;
        float lineWidthGranularity;
        uint32_t strictLines;
        uint32_t standardSampleLocations;
        uint64_t optimalBufferCopyOffsetAlignment;
        uint64_t optimalBufferCopyRowPitchAlignment;
        uint64_t nonCoherentAtomSize;

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PhysicalDeviceLimits);
#endif
    };

    struct DeviceSparseProperties
    {
        uint32_t residencyStandard2DBlockShape;
        uint32_t residencyStandard2DMultisampleBlockShape;
        uint32_t residencyStandard3DBlockShape;
        uint32_t residencyAlignedMipSize;
        uint32_t residencyNonResidentStrict;
#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PhysicalDeviceSparseProperties);
#endif
    };

    struct DeviceProperties
    {
        uint32_t apiVersion;
        uint32_t driverVersion;
        uint32_t vendorID;
        uint32_t deviceID;
        DeviceType deviceType;
        char deviceName[256];
        uint8_t pipelineCacheUUID[16];
        DeviceLimits limits;
        DeviceSparseProperties sparseProperties;

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PhysicalDeviceProperties);
#endif
    };

    struct ColorBlend
    {
        uint32_t blendEnable;
        BlendFactor srcColorBlendFactor;
        BlendFactor dstColorBlendFactor;
        BlendOp colorBlendOp;
        BlendFactor srcAlphaBlendFactor;
        BlendFactor dstAlphaBlendFactor;
        BlendOp alphaBlendOp;
        Flags<ColorComponent> colorWriteMask;

        ColorBlend() :
            blendEnable(false),
            srcColorBlendFactor(BlendFactor::Zero),
            dstColorBlendFactor(BlendFactor::Zero),
            colorBlendOp(BlendOp::Add),
            srcAlphaBlendFactor(BlendFactor::Zero),
            dstAlphaBlendFactor(BlendFactor::Zero),
            alphaBlendOp(BlendOp::Add),
            colorWriteMask(ColorComponent::RGBA)
        {}

        ColorBlend(
            BlendFactor srcColorBlendFactor,
            BlendFactor dstColorBlendFactor,
            BlendOp colorBlendOp,
            BlendFactor srcAlphaBlendFactor,
            BlendFactor dstAlphaBlendFactor,
            BlendOp alphaBlendOp,
            Flags<ColorComponent> colorWriteMask
        ) :
            blendEnable(true),
            srcColorBlendFactor(srcColorBlendFactor),
            dstColorBlendFactor(dstColorBlendFactor),
            colorBlendOp(colorBlendOp),
            srcAlphaBlendFactor(srcAlphaBlendFactor),
            dstAlphaBlendFactor(dstAlphaBlendFactor),
            alphaBlendOp(alphaBlendOp),
            colorWriteMask(colorWriteMask)
        {}

#ifdef VULKAN_HPP
        operator vk::PipelineColorBlendAttachmentState() const;
#endif
    };

    struct Attachment
    {
    private:
        uint32_t flags = 0;

    public:
        Format format;
        unsigned int samples;
        LoadOp loadOp;
        StoreOp storeOp;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        ImageLayout initialLayout;
        ImageLayout finalLayout;

        Attachment(
            Format format,
            ImageLayout finalLayout,
            ImageLayout initialLayout = ImageLayout::Undefined,
            LoadOp loadOp = LoadOp::Clear,
            StoreOp storeOp = StoreOp::Store,
            LoadOp stencilLoadOp = LoadOp::DontCare,
            StoreOp stencilStoreOp = StoreOp::DontCare,
            unsigned int samples = 1) :
            format(format), samples(samples), loadOp(loadOp), storeOp(storeOp), stencilLoadOp(stencilLoadOp), stencilStoreOp(stencilStoreOp), initialLayout(initialLayout), finalLayout(finalLayout)
        {}

#ifdef VULKAN_HPP
        operator vk::AttachmentDescription() const;
#endif
    };

    struct AttachmentReference
    {
        unsigned int index;
        ImageLayout layout;
        AttachmentReference(unsigned int index = 0, ImageLayout layout = ImageLayout::Undefined)
            :index(index), layout(layout)
        {}

#ifdef VULKAN_HPP
        operator vk::AttachmentReference() const;
#endif
    };

    struct Viewport
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
        Viewport() : Viewport(0, 0) {}
        Viewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1) :x(x), y(y), width(width), height(height), minDepth(minDepth), maxDepth(maxDepth) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(Viewport);
#endif
    };
    struct Scissor
    {
        int x;
        int y;
        int width;
        int height;
        Scissor() :Scissor(0, 0) {}
        Scissor(float width, float height, int x = 0, int y = 0) :x(x), y(y), width(width), height(height) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(Rect2D);
#endif
    };

    struct VertexBinding
    {
        uint32_t binding;
        uint32_t stride;
        InputRate inputRate;

        VertexBinding(int binding = 0, int stride = 0, InputRate inputRate = InputRate::Vertex) :binding(binding), stride(stride), inputRate(inputRate) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(VertexInputBindingDescription);
#endif
    };

    struct VertexAttribute
    {
        uint32_t location;
        uint32_t binding;
        Format format;
        uint32_t offset;

        VertexAttribute(uint32_t location = 0, uint32_t binding = 0, Format format = Format::Undefined, uint32_t offset = 0) :location(location), binding(binding), format(format), offset(offset) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(VertexInputAttributeDescription);
#endif
    };

    struct VertexLayout
    {
    private:
        const uint32_t reserved_1 = 19;
        const void* reserved_2 = nullptr;
        const uint32_t reserved_3 = 0;

    public:
        uint32_t vertexDescriptionCount = 0;
        const VertexBinding* vertexDescritpions = nullptr;
        uint32_t vertexAttributesCount = 0;
        const VertexAttribute* vertexAttributes = nullptr;

        VertexLayout(const std::vector<VertexBinding>& vertexDescriptions, const std::vector<VertexAttribute>& vertexAttributes)
            : vertexDescriptionCount(vertexDescriptions.size()), vertexDescritpions(new VertexBinding[vertexDescriptionCount]), vertexAttributesCount(vertexAttributes.size()), vertexAttributes(new VertexAttribute[vertexAttributesCount])
        {
            memcpy((void*) this->vertexDescritpions, &vertexDescriptions[0], sizeof(VertexBinding) * vertexDescriptionCount);
            memcpy((void*) this->vertexAttributes, &vertexAttributes[0], sizeof(VertexAttribute) * vertexAttributesCount);
        }

        VertexLayout(VertexLayout&& rhs) = default;
        VertexLayout& operator=(VertexLayout&& rhs) = default;

        VertexLayout(const VertexLayout& rhs)
        {
            vertexDescriptionCount = rhs.vertexDescriptionCount;
            vertexDescritpions = new VertexBinding[vertexDescriptionCount];
            memcpy((void*) vertexDescritpions, rhs.vertexDescritpions, sizeof(VertexBinding) * vertexDescriptionCount);

            vertexAttributesCount = rhs.vertexAttributesCount;
            vertexAttributes = new VertexAttribute[vertexAttributesCount];
            memcpy((void*) vertexAttributes, rhs.vertexAttributes, sizeof(VertexAttribute) * vertexAttributesCount);
        }
        VertexLayout& operator=(const VertexLayout& rhs)
        {
            if (&rhs == this)return *this;
            vertexDescriptionCount = rhs.vertexDescriptionCount;
            vertexDescritpions = new VertexBinding[vertexDescriptionCount];
            memcpy((void*) vertexDescritpions, rhs.vertexDescritpions, sizeof(VertexBinding) * vertexDescriptionCount);
            vertexAttributesCount = rhs.vertexAttributesCount;
            vertexAttributes = new VertexAttribute[vertexAttributesCount];
            memcpy((void*) vertexAttributes, rhs.vertexAttributes, sizeof(VertexAttribute) * vertexAttributesCount);
            return *this;
        }
        ~VertexLayout()
        {
            delete[] vertexDescritpions;
            delete[] vertexAttributes;
        }

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PipelineVertexInputStateCreateInfo);
#endif
    };
    struct InputAssembly
    {
    private:
        const uint32_t reserved_1 = 20;
        const void* reserved_2 = nullptr;
        const uint32_t reserved_3 = 0;

    public:
        Primitive primitive = Primitive::Points;
        uint32_t primitiveRestart = 0;
        InputAssembly(Primitive primitive = Primitive::Triangles, bool primitiveRestart = false) :primitive(primitive), primitiveRestart(primitiveRestart) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PipelineInputAssemblyStateCreateInfo);
#endif
    };
    struct Tesselation
    {
    private:
        const uint32_t reserved_1 = 21;
        const void* reserved_2 = nullptr;
        const uint32_t reserved_3 = 0;
        int reserved_4 = 0;

    public:
#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PipelineTessellationStateCreateInfo);
#endif
    };
    struct ViewportState
    {
    private:
        const uint32_t reserved_1 = 22;
        const void* reserved_2 = nullptr;
        const uint32_t reserved_3 = 0;

    public:
        uint32_t viewportCount = 0;
        Viewport* viewports = nullptr;
        uint32_t scissorCount = 0;
        Scissor* scissors = nullptr;
        ViewportState() :viewportCount(0), viewports(nullptr), scissorCount(0), scissors(nullptr) {}
        ViewportState(Viewport viewport, Scissor scissor) :viewportCount(1), viewports(new Viewport(viewport)), scissorCount(1), scissors(new Scissor(scissor)) {}
        ViewportState(std::vector<Viewport> viewports, std::vector<Scissor> scissors) :viewportCount(viewports.size()), viewports(new Viewport[viewportCount]), scissorCount(scissors.size()), scissors(new Scissor[scissorCount])
        {
            memcpy((void*) this->viewports, &viewports[0], sizeof(Viewport) * viewportCount);
            memcpy((void*) this->scissors, &scissors[0], sizeof(Scissor) * scissorCount);
        }

        ViewportState(ViewportState&& rhs) = default;
        ViewportState& operator=(ViewportState&& rhs) = default;

        ViewportState(const ViewportState& rhs)
        {
            viewportCount = rhs.viewportCount;
            viewports = new Viewport[viewportCount];
            memcpy((void*) viewports, rhs.viewports, sizeof(Viewport) * viewportCount);
            scissorCount = rhs.scissorCount;
            scissors = new Scissor[scissorCount];
            memcpy((void*) scissors, rhs.scissors, sizeof(Scissor) * scissorCount);
        }
        ViewportState& operator=(const ViewportState& rhs)
        {
            if (&rhs == this)return *this;
            viewportCount = rhs.viewportCount;
            viewports = new Viewport[viewportCount];
            memcpy((void*) viewports, rhs.viewports, sizeof(Viewport) * viewportCount);
            scissorCount = rhs.scissorCount;
            scissors = new Scissor[scissorCount];
            memcpy((void*) scissors, rhs.scissors, sizeof(Scissor) * scissorCount);
            return *this;
        }
        ~ViewportState()
        {
            delete[] viewports;
            delete[] scissors;
        }

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PipelineViewportStateCreateInfo);
#endif
    };
    struct alignas(float) DepthBias
    {
        uint32_t enable = 0;
        float constantFactor = 0;
        float clamp = 0;
        float slopeFactor = 0;
        DepthBias(bool enable = false, float constantFactor = 0, float clamp = 0, float slopeFactor = 0) :enable(enable), constantFactor(constantFactor), clamp(clamp), slopeFactor(slopeFactor) {}
    };
    struct Rasterizer
    {
    private:
        const uint32_t reserved_1 = 23;
        const void* reserved_2 = nullptr;
        const uint32_t reserved_3 = 0;

    public:
        uint32_t depthClamp = 0;
        uint32_t discard = 0;
        PolygonMode polygonMode = PolygonMode::Fill;
        CullMode cullMode = CullMode::None;
        FrontFace  frontFace = FrontFace::CounterClockwise;
        DepthBias depthBias;
        float lineWidth = 0;

        Rasterizer(bool enable, bool depthClamp, PolygonMode polygonMode, CullMode cullMode, FrontFace frontFace, DepthBias depthBias, float lineWidth)
            :discard(!enable), depthClamp(depthClamp), polygonMode(polygonMode), cullMode(cullMode), frontFace(frontFace), depthBias(depthBias), lineWidth(lineWidth)
        {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PipelineRasterizationStateCreateInfo);
#endif
    };
    struct Multisampling
    {
    private:
        const uint32_t reserved_1 = 24;
        const void* reserved_2 = nullptr;
        const uint32_t reserved_3 = 0;
        uint32_t rasterizationSamples = 1;
        uint32_t sampleShadingEnable = 0;
        float minSampleShading = 0;
        const void* pSampleMask = nullptr;
        uint32_t alphaToCoverageEnable = 0;
        uint32_t alphaToOneEnable = 0;

    public:
#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PipelineMultisampleStateCreateInfo);
#endif
    };
    struct DepthStencil
    {
    private:
        const uint32_t reserved_1 = 25;
        const void* reserved_2 = nullptr;
        const uint32_t reserved_3 = 0;
    public:
        uint32_t depthTestEnable = 0;
        uint32_t depthWriteEnable = 0;
        CompareOp depthCompareOp = CompareOp::Never;
        uint32_t depthBoundsTestEnable = 0;
        uint32_t stencilTestEnable = 0;
        uint32_t front = 0;
        uint32_t back = 0;
        float minDepthBounds = 0;
        float maxDepthBounds = 0;

    public:
        DepthStencil() {}
        DepthStencil(bool enableDepthTest, bool enableDepthWrite, CompareOp depthCompareOp) :depthTestEnable(enableDepthTest), depthWriteEnable(enableDepthWrite), depthCompareOp(depthCompareOp) {}
        DepthStencil(bool enableDepthTest, bool enableDepthWrite, CompareOp depthCompareOp, bool enabledepthBoundsTest, float minDepthBounds, float maxDepthBounds) :depthTestEnable(enableDepthTest), depthWriteEnable(enableDepthWrite), depthCompareOp(depthCompareOp), depthBoundsTestEnable(enabledepthBoundsTest), minDepthBounds(minDepthBounds), maxDepthBounds(maxDepthBounds) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PipelineDepthStencilStateCreateInfo);
#endif
    };
    struct ColorBlending
    {
    private:
        const uint32_t reserved_1 = 26;
        const void* reserved_2 = nullptr;
        const uint32_t reserved_3 = 0;

    public:
        uint32_t enableLogicOp = 0;
        LogicOp logicOp = LogicOp::Clear;
        uint32_t attachmentCount = 0;
        ColorBlend* attachments = nullptr;
        float blendConsts[4];

        ColorBlending(bool enableLogicOp, LogicOp logicOp, const float(&blendConstants)[4], const std::vector<ColorBlend>& colorBlending = {})
            :enableLogicOp(enableLogicOp), logicOp(logicOp), blendConsts{ blendConstants[0],blendConstants[1],blendConstants[2],blendConstants[3] }, attachmentCount(colorBlending.size()), attachments(new ColorBlend[attachmentCount])
        {
            memcpy((void*) this->attachments, &attachments[0], sizeof(ColorBlend) * attachmentCount);
        }

        ColorBlending(ColorBlending&& rhs) = default;
        ColorBlending& operator=(ColorBlending&& rhs) = default;

        ColorBlending(const ColorBlending& rhs)
        {
            enableLogicOp = rhs.enableLogicOp;
            logicOp = rhs.logicOp;
            memcpy(blendConsts, rhs.blendConsts, sizeof(float) * 4);
            attachmentCount = rhs.attachmentCount;
            attachments = new ColorBlend[attachmentCount];
            memcpy((void*) attachments, rhs.attachments, sizeof(ColorBlend) * attachmentCount);
        }
        ColorBlending& operator=(const ColorBlending& rhs)
        {
            if (&rhs == this)return *this;
            enableLogicOp = rhs.enableLogicOp;
            logicOp = rhs.logicOp;
            memcpy(blendConsts, rhs.blendConsts, sizeof(float) * 4);
            attachmentCount = rhs.attachmentCount;
            attachments = new ColorBlend[attachmentCount];
            memcpy((void*) attachments, rhs.attachments, sizeof(ColorBlend) * attachmentCount);
            return *this;
        }
        ~ColorBlending()
        {
            delete[] attachments;
        }

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(PipelineColorBlendStateCreateInfo);
#endif
    };

    struct DescriptorSetLayoutBinding
    {
    public:
        uint32_t binding;
        DescriptorType descriptorType;
        uint32_t descriptorCount;
        Flags<ShaderStage> stageFlags;
        const void* pImmutableSamplers = nullptr;

        DescriptorSetLayoutBinding(uint32_t binding, DescriptorType descriptorType, uint32_t descriptorCount, Flags<ShaderStage> stageFlags) :binding(binding), descriptorType(descriptorType), descriptorCount(descriptorCount), stageFlags(stageFlags)
        {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(DescriptorSetLayoutBinding);
#endif

    };

    struct BufferCopyRegion
    {
        uint64_t srcOffset;
        uint64_t dstOffset;
        uint64_t size;

        BufferCopyRegion(uint64_t size, uint64_t srcOffset = 0, uint64_t dstOffset = 0) :srcOffset(srcOffset), dstOffset(dstOffset), size(size) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(BufferCopy);
#endif
    };

    struct ImageSubresourceLayers
    {
        Flags<ImageAspect> aspectMask;
        uint32_t mipLevel;
        uint32_t baseArrayLayer;
        uint32_t layerCount;

        ImageSubresourceLayers(Flags<ImageAspect> aspectMask, uint32_t mipLevel = 0, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1)
            :aspectMask(aspectMask), mipLevel(mipLevel), baseArrayLayer(baseArrayLayer), layerCount(layerCount)
        {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(ImageSubresourceLayers);
#endif
    };

    struct BufferImageCopyRegion
    {
        uint64_t bufferOffset;
        uint32_t bufferRowLength;
        uint32_t bufferImageHeight;
        ImageSubresourceLayers imageSubresource;
        uint32_t imageOffsetX;
        uint32_t imageOffsetY;
        uint32_t imageOffsetZ;
        uint32_t imageExtendX;
        uint32_t imageExtendY;
        uint32_t imageExtendZ;

        BufferImageCopyRegion(uint64_t bufferOffset, uint32_t bufferRowLength, uint32_t bufferImageHeight, ImageSubresourceLayers imageSubresource, uint32_t imageExtendX, uint32_t imageExtendY, uint32_t imageExtendZ, uint32_t imageOffsetX = 0, uint32_t imageOffsetY = 0, uint32_t imageOffsetZ = 0)
            :bufferOffset(bufferOffset), bufferRowLength(bufferRowLength), bufferImageHeight(bufferImageHeight), imageSubresource(imageSubresource), imageOffsetX(imageOffsetX), imageOffsetY(imageOffsetY), imageOffsetZ(imageOffsetZ), imageExtendX(imageExtendX), imageExtendY(imageExtendY), imageExtendZ(imageExtendZ)
        {}

        BufferImageCopyRegion(uint64_t bufferOffset, ImageSubresourceLayers imageSubresource, uint32_t imageExtendX, uint32_t imageExtendY, uint32_t imageExtendZ, uint32_t imageOffsetX = 0, uint32_t imageOffsetY = 0, uint32_t imageOffsetZ = 0)
            :bufferOffset(bufferOffset), bufferRowLength(0), bufferImageHeight(0), imageSubresource(imageSubresource), imageOffsetX(imageOffsetX), imageOffsetY(imageOffsetY), imageOffsetZ(imageOffsetZ), imageExtendX(imageExtendX), imageExtendY(imageExtendY), imageExtendZ(imageExtendZ)
        {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(BufferImageCopy);
#endif
    };

    struct DescriptorPoolSize
    {
        DescriptorType descriptorType;
        uint32_t descriptorCount;

        DescriptorPoolSize(DescriptorType descriptorType, uint32_t descriptorCount) :descriptorType(descriptorType), descriptorCount(descriptorCount) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(DescriptorPoolSize);
#endif
    };

    struct ImageSubresourceRange
    {
        ImageAspect aspectMask;
        uint32_t baseMipLevel;
        uint32_t levelCount;
        uint32_t baseArrayLayer;
        uint32_t layerCount;

        ImageSubresourceRange(ImageAspect aspectMask, uint32_t baseMipLevel = 0, uint32_t levelCount = 1, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1)
            : aspectMask(aspectMask), baseMipLevel(baseMipLevel), levelCount(levelCount), baseArrayLayer(baseArrayLayer), layerCount(layerCount)
        {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(ImageSubresourceRange);
#endif
    };

    struct BufferMemoryBarrier
    {
    private:
        uint32_t sType = 44;
        void* pNext = nullptr;

    public:
        Flags<Access> srcAccessMask;
        Flags<Access> dstAccessMask;
        uint32_t srcQueueFamilyIndex;
        uint32_t dstQueueFamilyIndex;
        BufferHandle buffer;
        uint64_t offset;
        uint64_t size;

        BufferMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, const Queue& srcQueue, const Queue& dstQueue, BufferHandle buffer, uint64_t offset, uint64_t size)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), srcQueueFamilyIndex(srcQueue.GetIndex()), dstQueueFamilyIndex(dstQueue.GetIndex()), buffer(buffer), offset(offset), size(size)
        {}
        BufferMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, const Queue& dstQueue, BufferHandle buffer, uint64_t offset, uint64_t size)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), srcQueueFamilyIndex(~0U), dstQueueFamilyIndex(dstQueue.GetIndex()), buffer(buffer), offset(offset), size(size)
        {}
        BufferMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, BufferHandle buffer, uint64_t offset, uint64_t size)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), srcQueueFamilyIndex(~0U), dstQueueFamilyIndex(~0U), buffer(buffer), offset(offset), size(size)
        {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(BufferMemoryBarrier);
#endif
    };

    struct ImageMemoryBarrier
    {
    private:
        uint32_t sType = 45;
        void* pNext = nullptr;

    public:
        Flags<Access> srcAccessMask;
        Flags<Access> dstAccessMask;
        ImageLayout oldLayout;
        ImageLayout newLayout;
        uint32_t srcQueueFamilyIndex;
        uint32_t dstQueueFamilyIndex;
        ImageHandle image;
        ImageSubresourceRange subresourceRange;

        ImageMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, ImageLayout oldLayout, ImageLayout newLayout, const Queue& srcQueue, const Queue& dstQueue, ImageHandle image, ImageSubresourceRange subresourceRange)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), oldLayout(oldLayout), newLayout(newLayout), srcQueueFamilyIndex(srcQueue.GetIndex()), dstQueueFamilyIndex(dstQueue.GetIndex()), image(image), subresourceRange(subresourceRange)
        {}
        ImageMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, ImageLayout oldLayout, ImageLayout newLayout, const Queue& dstQueue, ImageHandle image, ImageSubresourceRange subresourceRange)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), oldLayout(oldLayout), newLayout(newLayout), srcQueueFamilyIndex(~0U), dstQueueFamilyIndex(dstQueue.GetIndex()), image(image), subresourceRange(subresourceRange)
        {}
        ImageMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, ImageLayout oldLayout, ImageLayout newLayout, ImageHandle image, ImageSubresourceRange subresourceRange)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), oldLayout(oldLayout), newLayout(newLayout), srcQueueFamilyIndex(~0U), dstQueueFamilyIndex(~0U), image(image), subresourceRange(subresourceRange)
        {}
        ImageMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, ImageLayout newLayout, const Queue& srcQueue, const Queue& dstQueue, ImageHandle image, ImageSubresourceRange subresourceRange)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), oldLayout(oldLayout), newLayout(newLayout), srcQueueFamilyIndex(srcQueue.GetIndex()), dstQueueFamilyIndex(dstQueue.GetIndex()), image(image), subresourceRange(subresourceRange)
        {}
        ImageMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, ImageLayout newLayout, const Queue& dstQueue, ImageHandle image, ImageSubresourceRange subresourceRange)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), oldLayout(oldLayout), newLayout(newLayout), srcQueueFamilyIndex(~0U), dstQueueFamilyIndex(dstQueue.GetIndex()), image(image), subresourceRange(subresourceRange)
        {}
        ImageMemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask, ImageLayout newLayout, ImageHandle image, ImageSubresourceRange subresourceRange)
            :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask), oldLayout(ImageLayout::Undefined), newLayout(newLayout), srcQueueFamilyIndex(~0U), dstQueueFamilyIndex(~0U), image(image), subresourceRange(subresourceRange)
        {}


#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(ImageMemoryBarrier);
#endif
    };

    struct MemoryBarrier
    {
    private:
        uint32_t sType = 46;
        void* pNext = nullptr;

    public:
        Flags<Access> srcAccessMask;
        Flags<Access> dstAccessMask;

        MemoryBarrier(Flags<Access> srcAccessMask, Flags<Access> dstAccessMask) :srcAccessMask(srcAccessMask), dstAccessMask(dstAccessMask) {}

#ifdef VULKAN_HPP
        VULKAN_NATIVE_CAST_OPERATOR(MemoryBarrier);
#endif
    };
}