#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <fstream>
#include <thread>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <math.h>
#include "Instance.h"
#include "Device.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "CmdBuffer.h"
#include "Synchronization.h"
#include "Flags.h"
#include "Buffer.h"
#include "Image.h"
#include "MemoryManager.h"
#include "Window.h"
#include "DescriptorPool.h"
#include "ImageView.h"
#include "Sampler.h"
#include "CmdPool.h"
#include "ComputePipeline.h"
#include "PipelineCache.h"

using namespace std::chrono_literals;
using namespace vg;
bool recreateFramebuffer = false;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VertexBinding& GetBindingDescription()
    {
        static VertexBinding bindingDescription(0, sizeof(Vertex));

        return bindingDescription;
    }
    static std::vector<VertexAttribute>& GetAttributeDescriptions()
    {
        static std::vector<VertexAttribute> attributeDescriptions = {
            VertexAttribute(0,0, Format::RGB32SFLOAT,offsetof(Vertex,position)),
            VertexAttribute(1,0, Format::RGB32SFLOAT,offsetof(Vertex,color)),
            VertexAttribute(2,0, Format::RG32SFLOAT,offsetof(Vertex,texCoord)),
        };

        return attributeDescriptions;
    }
};

struct Particle
{
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VertexBinding& GetBindingDescription()
    {
        static VertexBinding bindingDescription(0, sizeof(Particle));

        return bindingDescription;
    }
    static std::vector<VertexAttribute>& GetAttributeDescriptions()
    {
        static std::vector<VertexAttribute> attributeDescriptions = {
            VertexAttribute(0,0, Format::RG32SFLOAT,offsetof(Particle,position)),
            VertexAttribute(1,0, Format::RGBA32SFLOAT,offsetof(Particle,color))
        };

        return attributeDescriptions;
    }
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

int main()
{
    // Note: Swapchain constructor options.
    // Note: Better image and attachment constructor with msaaSamples.
    // Note: Better staging buffer uploading.
    // Note: Better image uploading.
    // Note: Maybe resize images when swapchain resizes.

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1600, 900, "Vulkan", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) {recreateFramebuffer = true; });
    int w, h; glfwGetFramebufferSize(window, &w, &h);

    vg::instance = Instance({ "VK_KHR_surface", "VK_KHR_win32_surface" },
        [](Debug::Severity severity, const char* message) {
            if (severity < Debug::Severity::Warning) return;
            std::cout << message << '\n' << '\n';
        });

    SurfaceHandle windowSurface = Window::CreateWindowSurface(vg::instance, window);
    DeviceFeatures deviceFeatures({ Feature::WideLines,Feature::LogicOp,Feature::SamplerAnisotropy,Feature::SampleRateShading });
    Queue generalQueue({ QueueType::General }, 1.0f);
    Device rendererDevice({ &generalQueue }, { "VK_KHR_swapchain" }, deviceFeatures, windowSurface,
        [](auto id, auto supportedQueues, auto supportedExtensions, auto type, DeviceLimits limits, DeviceFeatures features) {
            return (type == DeviceType::Dedicated);
        });
    vg::currentDevice = &rendererDevice;

    uint32_t msaaSampleCount = std::max(currentDevice->GetLimits().framebufferColorSampleCounts, currentDevice->GetLimits().framebufferDepthSampleCounts);
    msaaSampleCount = (msaaSampleCount >> 1) ^ msaaSampleCount;

    Surface surface(windowSurface, { Format::BGRA8SRGB, ColorSpace::SRGBNL });
    Swapchain swapchain(surface, 2, w, h);

    Image colorImage(swapchain.GetWidth(), swapchain.GetHeight(), surface.GetFormat(), { ImageUsage::ColorAttachment , ImageUsage::TransientAttachment }, 1, 1, ImageTiling::Optimal, ImageLayout::Undefined, msaaSampleCount);
    Image depthImage(swapchain.GetWidth(), swapchain.GetHeight(), { Format::D32SFLOAT,Format::D32SFLOATS8UINT,Format::x8D24UNORMPACK }, { FormatFeature::DepthStencilAttachment }, { ImageUsage::DepthStencilAttachment }, 1, 1, ImageTiling::Optimal, ImageLayout::Undefined, msaaSampleCount);
    Allocate({ &depthImage, &colorImage }, { MemoryProperty::DeviceLocal });
    ImageView colorImageView(colorImage, { ImageAspect::Color });
    ImageView depthImageView(depthImage, { ImageAspect::Depth });

    PipelineCache pipelineCache("pipelineCache.txt");
    Shader vertexShader(ShaderStage::Vertex, "resources/shaders/shader.vert.spv");
    Shader fragmentShader(ShaderStage::Fragment, "resources/shaders/shader.frag.spv");
    Shader vertexParticle(ShaderStage::Vertex, "resources/shaders/particle.vert.spv");
    Shader fragmentParticle(ShaderStage::Fragment, "resources/shaders/particle.frag.spv");
    RenderPass renderPass(
        {
            Attachment(surface.GetFormat(), ImageLayout::ColorAttachmentOptimal,ImageLayout::Undefined,LoadOp::Clear,StoreOp::Store,LoadOp::DontCare,StoreOp::DontCare,msaaSampleCount),
            Attachment(depthImage.GetFormat(), ImageLayout::DepthStencilAttachmentOptimal,ImageLayout::Undefined,LoadOp::Clear,StoreOp::Store,LoadOp::DontCare,StoreOp::DontCare,msaaSampleCount),
            Attachment(surface.GetFormat(), ImageLayout::PresentSrc)
        },
        {
            Subpass(
                GraphicsPipeline(
                    {{0, DescriptorType::UniformBuffer, 1, ShaderStage::Vertex},
                     {1, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment}},
                    {PushConstantRange({ShaderStage::Vertex},0,sizeof(glm::vec3))},
                    {&vertexShader, &fragmentShader},
                    VertexLayout({Vertex::GetBindingDescription()},Vertex::GetAttributeDescriptions()),
                    InputAssembly(Primitive::Triangles),
                    Tesselation(),
                    ViewportState(Viewport(w, h), Scissor(w, h)),
                    Rasterizer(false, PolygonMode::Fill, CullMode::None),
                    Multisampling(msaaSampleCount,true),
                    DepthStencil(true,true,CompareOp::Less),
                    ColorBlending(true, LogicOp::Copy, { 0,0,0,0 }, {ColorBlend()}),
                    { DynamicState::Viewport, DynamicState::Scissor},
                    1
                ),
                {}, {AttachmentReference(0, ImageLayout::ColorAttachmentOptimal)},
                {AttachmentReference(2,ImageLayout::ColorAttachmentOptimal)},
                 AttachmentReference(1, ImageLayout::DepthStencilAttachmentOptimal)
            ),
            Subpass(
                GraphicsPipeline(
                    0U,
                    std::nullopt,
                    std::nullopt,
                    std::vector<Shader*>{&vertexParticle,& fragmentParticle},
                    VertexLayout({Particle::GetBindingDescription()},Particle::GetAttributeDescriptions()),
                    InputAssembly(Primitive::Points)
                ),
                {}, {AttachmentReference(0, ImageLayout::ColorAttachmentOptimal)},
                {AttachmentReference(2,ImageLayout::ColorAttachmentOptimal)},
                 AttachmentReference(1, ImageLayout::DepthStencilAttachmentOptimal)
            )
        },
        {
            SubpassDependency(-1, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput, 0, Access::ColorAttachmentWrite, {}),
            SubpassDependency(0, 1, PipelineStage::ColorAttachmentOutput, PipelineStage::TopOfPipe, Access::ColorAttachmentWrite, 0, {})
        },
        pipelineCache
    );
    std::ofstream("pipelineCache.txt", std::ios_base::binary).write(pipelineCache.GetData().data(), pipelineCache.GetData().size());

    std::vector<Framebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(swapchain.GetImageCount());
    for (int i = 0; i < swapchain.GetImageCount(); i++)
        swapChainFramebuffers[i] = Framebuffer(renderPass, { colorImageView, depthImageView,swapchain.GetImageViews()[i] }, swapchain.GetWidth(), swapchain.GetHeight());

    // Allocate buffer in DeviceLocal memory.
    Buffer vertexBuffer(sizeof(vertices[0]) * vertices.size() + sizeof(indices[0]) * indices.size(), { vg::BufferUsage::VertexBuffer,vg::BufferUsage::IndexBuffer, vg::BufferUsage::TransferDst });
    vg::Allocate({ &vertexBuffer }, { MemoryProperty::DeviceLocal });
    {
        // Allocate staging buffer in HostVisible memory.
        Buffer stagingBuffer(vertexBuffer.GetSize(), vg::BufferUsage::TransferSrc);
        vg::Allocate({ &stagingBuffer }, { MemoryProperty::HostVisible });
        char* data = (char*) stagingBuffer.GetMappedMemory();
        memcpy(data, vertices.data(), sizeof(vertices[0]) * vertices.size());
        memcpy(data + sizeof(vertices[0]) * vertices.size(), indices.data(), sizeof(indices[0]) * indices.size());
        stagingBuffer.UnmapMemory();

        // Copy staging data to vertex buffer
        vg::CmdBuffer(generalQueue).Begin().Append(
            cmd::CopyBuffer(stagingBuffer, vertexBuffer, { BufferCopyRegion {vertexBuffer.GetSize() } })
        ).End().Submit().Await();
    }

    Buffer uniformBuffers(sizeof(UniformBufferObject) * swapchain.GetImageCount(), BufferUsage::UniformBuffer);
    vg::Allocate(&uniformBuffers, { MemoryProperty::HostVisible, MemoryProperty::HostCoherent });

    /// Load Image.
    int texWidth, texHeight, texChannels;
    unsigned char* pixels = stbi_load("resources/textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    Image texImage(texWidth, texHeight, Format::RGBA8SRGB, { ImageUsage::TransferDst,ImageUsage::TransferSrc , ImageUsage::Sampled }, -1);
    vg::Allocate(&texImage, { MemoryProperty::DeviceLocal });
    {
        // Upload data to GPU memory
        Buffer texStagingBuffer(texWidth * texHeight * 4, { BufferUsage::TransferSrc });
        vg::Allocate(&texStagingBuffer, { MemoryProperty::HostVisible, MemoryProperty::HostCoherent });
        memcpy(texStagingBuffer.GetMappedMemory(), pixels, texStagingBuffer.GetSize());
        stbi_image_free(pixels);

        vg::CmdBuffer imageCommandBuffer(generalQueue);
        imageCommandBuffer.Begin().Append(
            cmd::PipelineBarier(PipelineStage::TopOfPipe, PipelineStage::Transfer, { {texImage, ImageLayout::TransferDstOptimal, Access::None, Access::TransferWrite, {ImageAspect::Color,0,texImage.GetMipLevels()}} }),
            cmd::CopyBufferToImage(texStagingBuffer, texImage, ImageLayout::TransferDstOptimal, { {0UL,{ImageAspect::Color,0}, {texWidth,texHeight} } })
        );
        texImage.AppendMipmapGenerationCommands(&imageCommandBuffer, texImage.GetMipLevels());
        imageCommandBuffer.Append(
            cmd::PipelineBarier(PipelineStage::Transfer, PipelineStage::FragmentShader, { {texImage, ImageLayout::TransferSrcOptimal, ImageLayout::ShaderReadOnlyOptimal, Access::TransferWrite, Access::ShaderRead,{ImageAspect::Color,0,texImage.GetMipLevels()}} })
        ).End().Submit().Await();
    }
    ImageView imageView(texImage, { ImageAspect::Color ,0,texImage.GetMipLevels() });
    Sampler sampler(currentDevice->GetLimits().maxSamplerAnisotropy, Filter::Linear, Filter::Linear);

    // Create descriptor pools
    DescriptorPool descriptorPool(swapchain.GetImageCount(), {
        {DescriptorType::UniformBuffer, swapchain.GetImageCount()},
        {DescriptorType::CombinedImageSampler, swapchain.GetImageCount()}
        });

    // Create and allocate descriptor set layouts.
    std::vector<vg::DescriptorSetLayoutHandle> layouts(swapchain.GetImageCount(), renderPass.GetPipelineLayouts()[0].GetDescriptorSets()[0]);
    auto descriptorSets = descriptorPool.Allocate(layouts);

    uint64_t offsetAlignment = currentDevice->GetLimits().minUniformBufferOffsetAlignment;
    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        descriptorSets[i].AttachBuffer(DescriptorType::UniformBuffer, uniformBuffers, (sizeof(UniformBufferObject) * i / offsetAlignment) * offsetAlignment, sizeof(UniformBufferObject), 0, 0);
        descriptorSets[i].AttachImage(DescriptorType::CombinedImageSampler, ImageLayout::ShaderReadOnlyOptimal, imageView, sampler, 1, 0);
    }

    /// Compute part
    Queue computeQueue({ QueueType::Compute }, 1);
    Device computeDevice({ &computeQueue }, {}, {},
        [](PhysicalDeviceHandle id, auto supportedQueues, auto supportedExtensions, auto type, DeviceLimits limits, DeviceFeatures features) {
            return id != currentDevice;
        });
    ScopedDeviceChange aslkdja(&computeDevice);

    // Initialize Buffers.
    const int particleCount = 1024 * 32;
    std::vector<Buffer> shaderStorageBuffers;
    shaderStorageBuffers.resize(swapchain.GetImageCount());
    {
        // Initialize particles
        std::default_random_engine rndEngine((unsigned) time(nullptr));
        std::uniform_real_distribution<float> rndDist(-0.2f, 0.2f);

        // Initial particle positions on a circle
        std::vector<Particle> particles(particleCount);
        for (auto& particle : particles)
        {
            float x = rndDist(rndEngine);
            float y = rndDist(rndEngine);
            particle.position = glm::vec2(0, 0);
            particle.velocity = glm::vec2(rndDist(rndEngine) * 0.01, rndDist(rndEngine) * 0.01);
            particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        for (size_t i = 0; i < swapchain.GetImageCount(); i++)
            shaderStorageBuffers[i] = Buffer(sizeof(Particle) * particleCount, { BufferUsage::StorageBuffer, BufferUsage::VertexBuffer, BufferUsage::TransferDst });
        vg::Allocate(shaderStorageBuffers, { MemoryProperty::HostVisible ,MemoryProperty::HostCoherent });
        for (size_t i = 0; i < swapchain.GetImageCount(); i++)
            memcpy(shaderStorageBuffers[i].GetMappedMemory(), particles.data(), sizeof(Particle) * particleCount);
    }

    Shader computeShader(ShaderStage::Compute, "resources/shaders/shader.comp.spv");
    ComputePipeline computePipeline(computeShader,
        {
            DescriptorSetLayoutBinding(0,DescriptorType::StorageBuffer,1,ShaderStage::Compute),
            DescriptorSetLayoutBinding(1,DescriptorType::StorageBuffer,1,ShaderStage::Compute)
        }, {});

    DescriptorPool descriptorPool1(swapchain.GetImageCount(), {
        {DescriptorType::StorageBuffer, swapchain.GetImageCount() * 2}
        });
    auto descriptorSets1 = descriptorPool1.Allocate(std::vector<vg::DescriptorSetLayoutHandle>(swapchain.GetImageCount(), computePipeline.GetPipelineLayout().GetDescriptorSets()[0]));

    for (size_t i = 0; i < descriptorSets1.size(); i++)
    {
        descriptorSets1[i].AttachBuffer(DescriptorType::StorageBuffer, shaderStorageBuffers[(i - 1) % swapchain.GetImageCount()], 0, shaderStorageBuffers[i].GetSize(), 0, 0);
        descriptorSets1[i].AttachBuffer(DescriptorType::StorageBuffer, shaderStorageBuffers[i], 0, shaderStorageBuffers[i].GetSize(), 1, 0);
    }
    CmdBuffer computeCmdBuffer(computeQueue);
    ScopedDeviceChange aslkdja1(&rendererDevice);
    Buffer particleBuffer(sizeof(Particle) * particleCount, { BufferUsage::VertexBuffer });
    vg::Allocate(&particleBuffer, { MemoryProperty::HostVisible, MemoryProperty::HostCoherent });

    CmdBuffer commandBuffer(generalQueue);
    Semaphore renderFinishedSemaphore, imageAvailableSemaphore;
    Fence inFlightFence(true);
    UniformBufferObject ubo{ glm::mat4(1.0f) };
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        Fence::AwaitAll({ inFlightFence }, true);

        // Swapchain resize.
        Swapchain oldSwapchain;
        if (recreateFramebuffer)
        {
            std::swap(oldSwapchain, swapchain);
            swapchain = Swapchain(surface, 2, w, h, Usage::ColorAttachment, PresentMode::Fifo, CompositeAlpha::Opaque, oldSwapchain);
            colorImage = Image(swapchain.GetWidth(), swapchain.GetHeight(), surface.GetFormat(), { ImageUsage::ColorAttachment , ImageUsage::TransientAttachment }, 1, 1, ImageTiling::Optimal, ImageLayout::Undefined, msaaSampleCount);
            depthImage = Image(swapchain.GetWidth(), swapchain.GetHeight(), depthImage.GetFormat(), { ImageUsage::DepthStencilAttachment }, 1, 1, ImageTiling::Optimal, ImageLayout::Undefined, msaaSampleCount);
            Allocate({ &depthImage, &colorImage }, { MemoryProperty::DeviceLocal });
            colorImageView = ImageView(colorImage, { ImageAspect::Color });
            depthImageView = ImageView(depthImage, { ImageAspect::Depth });
            for (int i = 0; i < swapchain.GetImageCount(); i++)
                swapChainFramebuffers[i] = Framebuffer(renderPass, { colorImageView,depthImageView,swapchain.GetImageViews()[i] }, swapchain.GetWidth(), swapchain.GetHeight());
        }

        uint32_t imageIndex = swapchain.GetNextImageIndex(imageAvailableSemaphore);

        currentDevice = &computeDevice;
        computeCmdBuffer.Begin({}).Append(
            cmd::BindPipeline(computePipeline),
            cmd::BindDescriptorSets(computePipeline.GetPipelineLayout(), PipelineBindPoint::Compute, 0, { descriptorSets1[imageIndex] }),
            cmd::Dispatch(particleCount / 256, 1, 1)
        ).End().Submit().Await();
        currentDevice = &rendererDevice;
        memcpy(particleBuffer.GetMappedMemory(), shaderStorageBuffers[imageIndex].GetMappedMemory(), particleBuffer.GetSize());

        ubo.model = glm::rotate(ubo.model, glm::radians(0.3f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), swapchain.GetWidth() / (float) swapchain.GetHeight(), 0.01f, 100.0f); ubo.proj[1][1] *= -1;
        memcpy(uniformBuffers.GetMappedMemory() + imageIndex * sizeof(ubo), &ubo, sizeof(ubo));

        commandBuffer.Clear().Begin().Append(
            cmd::BeginRenderpass(renderPass, swapChainFramebuffers[imageIndex], { 0, 0 }, { swapchain.GetWidth(), swapchain.GetHeight() }, { ClearColor{ 0,0,0,255 },ClearDepthStencil{1.0f,0U},ClearColor{ 0,0,0,255 } }),
            cmd::BindPipeline(renderPass.GetPipelines()[0]),
            cmd::BindDescriptorSets(renderPass.GetPipelineLayouts()[0], PipelineBindPoint::Graphics, 0, { descriptorSets[imageIndex] }),
            cmd::BindVertexBuffer(vertexBuffer, 0),
            cmd::BindIndexBuffer(vertexBuffer, sizeof(vertices[0]) * vertices.size(), IndexType::Uint16),
            cmd::SetViewport(Viewport(swapchain.GetWidth(), swapchain.GetHeight())),
            cmd::SetScissor(Scissor(swapchain.GetWidth(), swapchain.GetHeight())),
            cmd::PushConstants(renderPass.GetPipelineLayouts()[0], { ShaderStage::Vertex }, 0, glm::vec3(-2, 0, 0)),
            cmd::DrawIndexed(indices.size()),
            cmd::PushConstants(renderPass.GetPipelineLayouts()[0], { ShaderStage::Vertex }, 0, glm::vec3(0, 0, 0)),
            cmd::DrawIndexed(indices.size()),
            cmd::NextSubpass(),
            cmd::BindPipeline(renderPass.GetPipelines()[1]),
            cmd::BindVertexBuffer(particleBuffer, 0),
            cmd::Draw(particleCount),
            cmd::EndRenderpass()
        ).End().Submit({ {PipelineStage::ColorAttachmentOutput, imageAvailableSemaphore} }, { renderFinishedSemaphore }, inFlightFence);
        generalQueue.Present({ renderFinishedSemaphore }, { swapchain }, { imageIndex });
    }
    Fence::AwaitAll({ inFlightFence });
    glfwTerminate();
}