#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include <random>
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
#include "FormatInfo.h"
#include "CmdPool.h"
#include "PipelineCache.h"
#include "ComputePipeline.h"

using namespace std::chrono_literals;
namespace fs = std::filesystem;
using namespace vg;
bool recreateFramebuffer = false;

struct Particle
{
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VertexBinding& getBindingDescription()
    {
        static VertexBinding bindingDescription(0, sizeof(Particle));

        return bindingDescription;
    }
    static std::vector<VertexAttribute>& getAttributeDescriptions()
    {
        static std::vector<VertexAttribute> attributeDescriptions = {
            VertexAttribute(0,0, Format::RG32SFLOAT,offsetof(Particle,position)),
            VertexAttribute(1,0, Format::RGB32SFLOAT,offsetof(Particle,color))
        };

        return attributeDescriptions;
    }
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Vulkan", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) {recreateFramebuffer = true; });

    vg::instance = Instance({ "VK_KHR_surface", "VK_KHR_win32_surface" },
        [](Debug::Severity severity, const char* message) {
            if (severity < Debug::Severity::Warning) return;
            std::cout << message << '\n' << '\n';
        });

    SurfaceHandle windowSurface = Window::CreateWindowSurface(vg::instance, window);
    DeviceFeatures deviceFeatures;
    deviceFeatures.wideLines = true;
    deviceFeatures.logicOp = true;
    deviceFeatures.samplerAnisotropy = true;
    deviceFeatures.sampleRateShading = true;
    vg::currentDevice = Device({ QueueType::Graphics, QueueType::Present, QueueType::Transfer }, { "VK_KHR_swapchain" }, deviceFeatures, windowSurface,
        [&deviceFeatures](auto supportedQueues, auto supportedExtensions, auto type, DeviceLimits limits, DeviceFeatures features) {
            return (type == DeviceType::Dedicated) + 1 + (deviceFeatures == features);
        });

    Surface surface(windowSurface, Format::BGRA8SRGB, ColorSpace::SRGBNL);
    int w, h; glfwGetFramebufferSize(window, &w, &h);

    Swapchain swapchain(surface, 1, w, h, Usage::ColorAttachment, PresentMode::Fifo);
    Image depthImage(swapchain.GetWidth(), swapchain.GetHeight(), { Format::D32SFLOAT,Format::D32SFLOATS8UINT,Format::x8D24UNORMPACK }, { FormatFeature::DepthStencilAttachment }, { ImageUsage::DepthStencilAttachment });
    Allocate({ &depthImage }, { MemoryProperty::DeviceLocal });
    ImageView depthImageView(depthImage, { ImageAspect::Depth });

    Shader vertexShader(ShaderStage::Vertex, "resources/shaders/shader.vert.spv");
    Shader fragmentShader(ShaderStage::Fragment, "resources/shaders/shader.frag.spv");
    RenderPass renderPass(
        {
            Attachment(surface.GetFormat(), ImageLayout::PresentSrc),
            Attachment(depthImage.GetFormat(), ImageLayout::DepthStencilAttachmentOptimal),
        },
        {
            Subpass(
                GraphicsPipeline(
                    {},
                    {},
                    {&vertexShader, &fragmentShader},
                    VertexLayout({Particle::getBindingDescription()},Particle::getAttributeDescriptions()),
                    InputAssembly(Primitive::Points),
                    Tesselation(),
                    ViewportState(Viewport(w, h), Scissor(w, h)),
                    Rasterizer(false, PolygonMode::Fill, CullMode::None),
                    Multisampling(1),
                    DepthStencil(true,true,CompareOp::Less),
                    ColorBlending(false, LogicOp::Copy, { 0,0,0,0 }, {ColorBlend()}),
                    { DynamicState::Viewport, DynamicState::Scissor}
                ),
                {}, {AttachmentReference(0, ImageLayout::ColorAttachmentOptimal)},
                {}, AttachmentReference(1, ImageLayout::DepthStencilAttachmentOptimal)
            )
        },
        {
            SubpassDependency(~0, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput, 0, Access::ColorAttachmentWrite, {})
        }
    );

    std::vector<Framebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(swapchain.GetImageCount());
    for (int i = 0; i < swapchain.GetImageCount(); i++)
        swapChainFramebuffers[i] = Framebuffer(renderPass, { swapchain.GetImageViews()[i], depthImageView }, swapchain.GetWidth(), swapchain.GetHeight());

    /// Compute part

    // Initialize Buffers.
    const int particleCount = 4194304 / 4;
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

        Buffer stagingBuffer(sizeof(Particle) * particleCount, { BufferUsage::TransferSrc });
        vg::Allocate(&stagingBuffer, { MemoryProperty::HostVisible ,MemoryProperty::HostCoherent });

        void* data = stagingBuffer.MapMemory();
        memcpy(data, particles.data(), (size_t) stagingBuffer.GetSize());
        stagingBuffer.UnmapMemory();

        for (size_t i = 0; i < swapchain.GetImageCount(); i++)
            shaderStorageBuffers[i] = Buffer(stagingBuffer.GetSize(), { BufferUsage::StorageBuffer, BufferUsage::VertexBuffer, BufferUsage::TransferDst });

        vg::Allocate(shaderStorageBuffers, { MemoryProperty::DeviceLocal });

        CmdBuffer transfer(currentDevice.transferQueue);
        transfer.Begin();
        for (size_t i = 0; i < swapchain.GetImageCount(); i++)
            transfer.Append(cmd::CopyBuffer(stagingBuffer, shaderStorageBuffers[i], { {stagingBuffer.GetSize()} }));
        transfer.End().Submit().Await();
    }

    Shader computeShader(ShaderStage::Compute, "resources/shaders/shader.comp.spv");
    ComputePipeline computePipeline(computeShader,
        {
            DescriptorSetLayoutBinding(0,DescriptorType::StorageBuffer,1,ShaderStage::Compute),
            DescriptorSetLayoutBinding(1,DescriptorType::StorageBuffer,1,ShaderStage::Compute)
        }, {});

    // Create descriptor pools
    DescriptorPool descriptorPool(swapchain.GetImageCount(), {
        {DescriptorType::StorageBuffer, swapchain.GetImageCount() * 2}
        });
    // Create and allocate descriptor set layouts.
    auto descriptorSets = descriptorPool.Allocate(std::vector<vg::DescriptorSetLayoutHandle>(swapchain.GetImageCount(), computePipeline.GetPipelineLayout().GetDescriptorSets()[0]));

    uint64_t offsetAlignment = currentDevice.GetProperties().limits.minUniformBufferOffsetAlignment;
    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        descriptorSets[i].AttachBuffer(DescriptorType::StorageBuffer, shaderStorageBuffers[(i - 1) % swapchain.GetImageCount()], 0, shaderStorageBuffers[i].GetSize(), 0, 0);
        descriptorSets[i].AttachBuffer(DescriptorType::StorageBuffer, shaderStorageBuffers[i], 0, shaderStorageBuffers[i].GetSize(), 1, 0);
    }

    CmdBuffer computeCmdBuffer(currentDevice.computeQueue);
    CmdBuffer commandBuffer(currentDevice.graphicsQueue);
    Semaphore renderFinishedSemaphore;
    Semaphore imageAvailableSemaphore;
    Fence inFlightFence(true);
    auto startTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        // glfwFocusWindow(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        Fence::AwaitAll({ inFlightFence }, true);

        // Swapchain resize.
        Swapchain oldSwapchain;
        if (recreateFramebuffer)
        {
            std::swap(oldSwapchain, swapchain);
            swapchain = Swapchain(surface, 2, w, h, Usage::ColorAttachment, PresentMode::Fifo, CompositeAlpha::Opaque, oldSwapchain);
            depthImage = Image(swapchain.GetWidth(), swapchain.GetHeight(), depthImage.GetFormat(), { ImageUsage::DepthStencilAttachment });
            Allocate({ &depthImage }, { MemoryProperty::DeviceLocal });
            depthImageView = ImageView(depthImage, { ImageAspect::Depth });
            for (int i = 0; i < swapchain.GetImageCount(); i++)
                swapChainFramebuffers[i] = Framebuffer(renderPass, { swapchain.GetImageViews()[i] ,depthImageView }, swapchain.GetWidth(), swapchain.GetHeight());
        }

        uint32_t imageIndex = swapchain.GetNextImageIndex(imageAvailableSemaphore);

        computeCmdBuffer.Begin({}).Append(
            cmd::BindPipeline(computePipeline),
            cmd::BindDescriptorSets(computePipeline.GetPipelineLayout(), PipelineBindPoint::Compute, 0, { descriptorSets[imageIndex] }),
            cmd::Dispatch(particleCount / 256, 1, 1)
        ).End().Submit().Await();

        commandBuffer.Clear().Begin().Append(
            cmd::BeginRenderpass(renderPass, swapChainFramebuffers[imageIndex], { 0, 0 }, { swapchain.GetWidth(), swapchain.GetHeight() }, { 0,0,0,255 }),
            cmd::BindPipeline(renderPass.GetPipelines()[0]),
            cmd::SetViewport(Viewport(swapchain.GetWidth(), swapchain.GetHeight())),
            cmd::SetScissor(Scissor(swapchain.GetWidth(), swapchain.GetHeight())),
            cmd::BindVertexBuffer(shaderStorageBuffers[imageIndex], 0),
            cmd::Draw(particleCount),
            cmd::EndRenderpass()
        ).End().Submit({ {PipelineStage::ColorAttachmentOutput, imageAvailableSemaphore} }, { renderFinishedSemaphore }, inFlightFence);
        currentDevice.presentQueue.Present({ renderFinishedSemaphore }, { swapchain }, { imageIndex });
    }
    Fence::AwaitAll({ inFlightFence });
    glfwTerminate();
}