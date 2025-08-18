#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Buffer.h"
#include "CmdBuffer.h"
#include "CmdPool.h"
#include "ComputePipeline.h"
#include "DescriptorPool.h"
#include "Device.h"
#include "Flags.h"
#include "FormatInfo.h"
#include "Framebuffer.h"
#include "Image.h"
#include "ImageView.h"
#include "Instance.h"
#include "MemoryManager.h"
#include "PipelineCache.h"
#include "QueryPool.h"
#include "RenderPass.h"
#include "Sampler.h"
#include "Shader.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Synchronization.h"
#include <chrono>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <math.h>
#include <random>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <thread>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
extern "C" {
typedef struct VkInstance_T *VkInstance;
typedef struct VkSurfaceKHR_T *VkSurfaceKHR;
int glfwCreateWindowSurface(VkInstance instance, GLFWwindow *window, const void *allocator, VkSurfaceKHR *surface);
}

using namespace std::chrono_literals;
using namespace vg;
using namespace vg::cmd;

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VertexBinding &GetBindingDescription() {
        static VertexBinding bindingDescription(0, sizeof(Vertex));

        return bindingDescription;
    }
    static std::vector<VertexAttribute> &GetAttributeDescriptions() {
        static std::vector<VertexAttribute> attributeDescriptions = {
            VertexAttribute(0, 0, Format::RGB32SFLOAT, offsetof(Vertex, position)),
            VertexAttribute(1, 0, Format::RGB32SFLOAT, offsetof(Vertex, color)),
            VertexAttribute(2, 0, Format::RG32SFLOAT, offsetof(Vertex, texCoord)),
        };

        return attributeDescriptions;
    }
};

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VertexBinding &GetBindingDescription() {
        static VertexBinding bindingDescription(0, sizeof(Particle));

        return bindingDescription;
    }
    static std::vector<VertexAttribute> &GetAttributeDescriptions() {
        static std::vector<VertexAttribute> attributeDescriptions = {
            VertexAttribute(0, 0, Format::RG32SFLOAT, offsetof(Particle, position)),
            VertexAttribute(1, 0, Format::RGBA32SFLOAT, offsetof(Particle, color))
        };

        return attributeDescriptions;
    }
};

const std::vector<Vertex> vertices = {{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                      {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                      {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                      {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                                      {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                      {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                      {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                      {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

bool recreateFramebuffer = false;

struct Material {
    Subpass subpass;
    Material(Subpass &&subpass) : subpass(std::move(subpass)) {}
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *window = glfwCreateWindow(1600, 900, "Vulkan", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int w, int h) { recreateFramebuffer = true; });
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vg::instance = Instance({glfwExtensions, glfwExtensionCount}, [](MessageSeverity severity, const char *message) {
        if (severity < MessageSeverity::Warning) return;
        std::cout << message << '\n';
        return;
        std::string m(message);
        m = m.substr(m.find('[') + 2, m.size());
        std::string brackets = m.substr(0, m.find(']') - 1);
        m = m.substr(brackets.size(), m.size());
        m = m.substr(m.find(',') + 1, m.size());
        std::string type = m.substr(0, m.find(';'));
        m = m.substr(type.size() + 3, m.size());
        m = m.substr(m.find('|') + 1, m.size());
        m = m.substr(0, m.rfind('('));
        std::string me = m.substr(0, m.rfind('.'));
        m = m.substr(m.rfind(':') + 1, m.size());

        std::cout << type << me << '.' << m << '\n';
    });

    SurfaceHandle windowSurface;
    glfwCreateWindowSurface(
        *(VkInstance *)&vg::instance, (GLFWwindow *)window, nullptr, (VkSurfaceKHR *)&windowSurface
    );
    DeviceFeatures deviceFeatures(
        {Feature::WideLines, Feature::LogicOp, Feature::SamplerAnisotropy, Feature::SampleRateShading}
    );
    Queue generalQueue({QueueType::General}, 1.0f);
    Device rendererDevice(
        {&generalQueue}, {"VK_KHR_swapchain"}, deviceFeatures, windowSurface,
        [](auto id, auto supportedQueues, auto supportedExtensions, auto type, DeviceLimits limits,
           DeviceFeatures features) { return (type == DeviceType::Dedicated); }
    );
    vg::currentDevice = &rendererDevice;

    uint32_t msaaSampleCount = std::max(
        currentDevice->GetLimits().framebufferColorSampleCounts, currentDevice->GetLimits().framebufferDepthSampleCounts
    );
    msaaSampleCount = (msaaSampleCount >> 1) ^ msaaSampleCount;

    Surface surface(windowSurface, {Format::BGRA8SRGB, ColorSpace::SRGBNL});
    Swapchain swapchain(surface, 2, w, h);

    Image colorImage(
        {swapchain.GetWidth(), swapchain.GetHeight()}, surface.GetFormat(),
        {ImageUsage::ColorAttachment, ImageUsage::TransientAttachment}, 1, 1, msaaSampleCount
    );
    Image depthImage(
        {swapchain.GetWidth(), swapchain.GetHeight()},
        {Format::D32SFLOAT, Format::D32SFLOATS8UINT, Format::x8D24UNORMPACK}, {FormatFeature::DepthStencilAttachment},
        {ImageUsage::DepthStencilAttachment}, 1, 1, msaaSampleCount
    );
    Allocate(Span<Image *const>{&depthImage, &colorImage}, {MemoryProperty::DeviceLocal});
    ImageView colorImageView(colorImage, {ImageAspect::Color});
    ImageView depthImageView(depthImage, {ImageAspect::Depth});

    PipelineCache pipelineCache("pipelineCache.txt");
    Shader vertexShader(ShaderStage::Vertex, "resources/shaders/shader.vert.spv");
    Shader fragmentShader(ShaderStage::Fragment, "resources/shaders/shader.frag.spv");

    Subpass sub(
        GraphicsPipeline(
            0, std::vector<vg::Shader *>{&vertexShader, &fragmentShader},
            VertexLayout({Vertex::GetBindingDescription()}, Vertex::GetAttributeDescriptions()),
            InputAssembly(Primitive::Triangles), Tesselation(), ViewportState(Viewport(w, h), Scissor(w, h)),
            Rasterizer(false, false, PolygonMode::Fill, CullMode::None), Multisampling(msaaSampleCount, true),
            DepthStencil(true, true, CompareOp::Less),
            ColorBlending(
                true, LogicOp::Copy, {0, 0, 0, 0},
                {ColorBlend(
                    BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add, BlendFactor::One,
                    BlendFactor::Zero, BlendOp::Add, ColorComponent::RGBA
                )}
            ),
            {DynamicState::Viewport, DynamicState::Scissor}, 1
        ),
        {}, {AttachmentReference(0, ImageLayout::ColorAttachmentOptimal)},
        {AttachmentReference(2, ImageLayout::ColorAttachmentOptimal)},
        AttachmentReference(1, ImageLayout::DepthStencilAttachmentOptimal)
    );
    RenderPass testPass(
        {Attachment(surface.GetFormat(), msaaSampleCount, ImageLayout::ColorAttachmentOptimal),
         Attachment(depthImage.GetFormat(), msaaSampleCount, ImageLayout::DepthStencilAttachmentOptimal),
         Attachment(surface.GetFormat(), ImageLayout::PresentSrc)},
        Vector<PipelineLayout>{PipelineLayout{
            {{{0, DescriptorType::UniformBuffer, 1, ShaderStage::Vertex},
              {1, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment}}},
            {PushConstantRange({ShaderStage::Vertex}, 0, sizeof(glm::vec3))}
        }},
        std::span{&sub, &sub + 1},
        {SubpassDependency(
            -1, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput, 0,
            Access::ColorAttachmentWrite, {}
        )},
        pipelineCache
    );

    RenderPass renderPass(
        {Attachment(surface.GetFormat(), msaaSampleCount, ImageLayout::ColorAttachmentOptimal),
         Attachment(depthImage.GetFormat(), msaaSampleCount, ImageLayout::DepthStencilAttachmentOptimal),
         Attachment(surface.GetFormat(), ImageLayout::PresentSrc)},
        Vector<PipelineLayout>{
            PipelineLayout{
                {{{0, DescriptorType::UniformBuffer, 1, ShaderStage::Vertex},
                  {1, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment}}},
                {PushConstantRange({ShaderStage::Vertex}, 0, sizeof(glm::vec3))}
            },
        },
        {Subpass(
             GraphicsPipeline(
                 0, std::vector<vg::Shader *>{&vertexShader, &fragmentShader},
                 VertexLayout({Vertex::GetBindingDescription()}, Vertex::GetAttributeDescriptions()),
                 InputAssembly(Primitive::Triangles), Tesselation(), ViewportState(Viewport(w, h), Scissor(w, h)),
                 Rasterizer(false, false, PolygonMode::Fill, CullMode::None), Multisampling(msaaSampleCount, true),
                 DepthStencil(true, true, CompareOp::Less),
                 ColorBlending(
                     true, LogicOp::Copy, {0, 0, 0, 0},
                     {ColorBlend(
                         BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendOp::Add, BlendFactor::One,
                         BlendFactor::Zero, BlendOp::Add, ColorComponent::RGBA
                     )}
                 ),
                 {DynamicState::Viewport, DynamicState::Scissor}
             ),
             {}, {AttachmentReference(0, ImageLayout::ColorAttachmentOptimal)},
             {AttachmentReference(2, ImageLayout::ColorAttachmentOptimal)},
             AttachmentReference(1, ImageLayout::DepthStencilAttachmentOptimal)
         ),
         Subpass(
             GraphicsPipeline(
                 &sub.graphicsPipeline, std::nullopt,
                 Vector{
                     Shader(ShaderStage::Vertex, "resources/shaders/particle.vert.spv"),
                     Shader(ShaderStage::Fragment, "resources/shaders/particle.frag.spv")
                 },
                 VertexLayout({Particle::GetBindingDescription()}, Particle::GetAttributeDescriptions()),
                 InputAssembly(Primitive::Points)
             ),
             {}, {AttachmentReference(0, ImageLayout::ColorAttachmentOptimal)},
             {AttachmentReference(2, ImageLayout::ColorAttachmentOptimal)},
             AttachmentReference(1, ImageLayout::DepthStencilAttachmentOptimal)
         )},
        {SubpassDependency(
             -1, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput, 0,
             Access::ColorAttachmentWrite, {}
         ),
         SubpassDependency(
             0, 1, PipelineStage::ColorAttachmentOutput, PipelineStage::TopOfPipe, Access::ColorAttachmentWrite, 0, {}
         )},
        pipelineCache
    );
    std::vector<char> data = pipelineCache.GetData();
    std::ofstream("pipelineCache.txt", std::ios_base::binary | std::ios_base::out).write(data.data(), data.size());

    std::vector<Framebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(swapchain.GetImageCount());
    for (int i = 0; i < swapchain.GetImageCount(); i++)
        swapChainFramebuffers[i] = Framebuffer(
            renderPass, {colorImageView, depthImageView, swapchain.GetImageViews()[i]}, swapchain.GetWidth(),
            swapchain.GetHeight()
        );

    // Allocate buffer in DeviceLocal memory.
    Buffer vertexBuffer(
        sizeof(vertices[0]) * vertices.size() + sizeof(indices[0]) * indices.size(),
        {vg::BufferUsage::VertexBuffer, vg::BufferUsage::IndexBuffer, vg::BufferUsage::TransferDst}
    );
    vg::Allocate({&vertexBuffer}, {MemoryProperty::DeviceLocal});
    {
        // Allocate staging buffer in HostVisible memory.
        Buffer stagingBuffer(vertexBuffer.GetSize(), vg::BufferUsage::TransferSrc);
        vg::Allocate({&stagingBuffer}, {MemoryProperty::HostVisible});
        memcpy(stagingBuffer.MapMemory(), vertices.data(), sizeof(vertices[0]) * vertices.size());
        memcpy(
            stagingBuffer.MapMemory() + sizeof(vertices[0]) * vertices.size(), indices.data(),
            sizeof(indices[0]) * indices.size()
        );

        // Copy staging data to vertex buffer
        vg::CmdBuffer(generalQueue)
            .Begin()
            .Append(cmd::CopyBuffer(stagingBuffer, vertexBuffer, {BufferCopyRegion{vertexBuffer.GetSize()}}))
            .End()
            .Submit()
            .Await();
    }

    Buffer uniformBuffers(sizeof(UniformBufferObject) * swapchain.GetImageCount(), BufferUsage::UniformBuffer);
    vg::Allocate(&uniformBuffers, {MemoryProperty::HostVisible, MemoryProperty::HostCoherent});

    /// Load Image.
    int texWidth, texHeight, texChannels;
    unsigned char *pixels =
        stbi_load("resources/textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    Image texImage(
        {(uint32_t)texWidth, (uint32_t)texHeight}, Format::RGBA8SRGB,
        {ImageUsage::TransferDst, ImageUsage::TransferSrc, ImageUsage::Sampled}, -1
    );
    vg::Allocate(&texImage, {MemoryProperty::DeviceLocal});
    {
        // Upload data to GPU memory
        Buffer texStagingBuffer(
            texWidth * texHeight * GetFormatResolutions(texImage.GetFormat()) / 8, {BufferUsage::TransferSrc}
        );
        vg::Allocate(&texStagingBuffer, {MemoryProperty::HostVisible, MemoryProperty::HostCoherent});
        memcpy(texStagingBuffer.MapMemory(), pixels, texStagingBuffer.GetSize());
        stbi_image_free(pixels);

        vg::CmdBuffer imageCommandBuffer(generalQueue);
        imageCommandBuffer.Begin().Append(
            cmd::PipelineBarier(
                PipelineStage::TopOfPipe, PipelineStage::Transfer,
                {{texImage,
                  ImageLayout::TransferDstOptimal,
                  Access::None,
                  Access::TransferWrite,
                  {ImageAspect::Color, 0, texImage.GetMipLevels()}}}
            ),
            cmd::CopyBufferToImage(
                texStagingBuffer, texImage, ImageLayout::TransferDstOptimal,
                {{0UL, {ImageAspect::Color, 0}, {texWidth, texHeight}}}
            )
        );
        texImage.AppendMipmapGenerationCommands(&imageCommandBuffer, texImage.GetMipLevels());
        imageCommandBuffer
            .Append(
                cmd::PipelineBarier(
                    PipelineStage::Transfer, PipelineStage::FragmentShader,
                    {{texImage,
                      ImageLayout::TransferSrcOptimal,
                      ImageLayout::ShaderReadOnlyOptimal,
                      Access::TransferWrite,
                      Access::ShaderRead,
                      {ImageAspect::Color, 0, texImage.GetMipLevels()}}}
                )
            )
            .End()
            .Submit()
            .Await();
    }
    ImageView imageView(texImage, {ImageAspect::Color, 0, texImage.GetMipLevels()});
    Sampler sampler(currentDevice->GetLimits().maxSamplerAnisotropy, Filter::Linear, Filter::Linear);

    // Create descriptor pools
    DescriptorPool descriptorPool(
        swapchain.GetImageCount(), {{DescriptorType::UniformBuffer, swapchain.GetImageCount()},
                                    {DescriptorType::CombinedImageSampler, swapchain.GetImageCount()}}
    );

    // Create and allocate descriptor set layouts.
    std::vector<vg::DescriptorSetLayoutHandle> layouts(
        swapchain.GetImageCount(), renderPass.GetPipelineLayouts()[0].GetDescriptorSets()[0]
    );
    std::vector<vg::DescriptorSet> descriptorSets = descriptorPool.Allocate(layouts);

    uint64_t offsetAlignment = currentDevice->GetLimits().minUniformBufferOffsetAlignment;
    for (size_t i = 0; i < descriptorSets.size(); i++) {
        descriptorSets[i].AttachBuffer(
            DescriptorType::UniformBuffer, uniformBuffers,
            (sizeof(UniformBufferObject) * i / offsetAlignment) * offsetAlignment, sizeof(UniformBufferObject), 0, 0
        );
        descriptorSets[i].AttachImage(
            DescriptorType::CombinedImageSampler, ImageLayout::ShaderReadOnlyOptimal, imageView, sampler, 1, 0
        );
    }

    /// Compute part
    Queue computeQueue({QueueType::Compute}, 1);
    Device computeDevice(
        {&computeQueue}, {}, {},
        [](PhysicalDeviceHandle id, auto supportedQueues, auto supportedExtensions, auto type, DeviceLimits limits,
           DeviceFeatures features) { return id != *currentDevice; }
    );
    SCOPED_DEVICE_CHANGE(&computeDevice);

    // Initialize Buffers.
    const int particleCount = 1024 * 32;
    std::vector<Buffer> shaderStorageBuffers;
    shaderStorageBuffers.resize(swapchain.GetImageCount());
    {
        // Initialize particles
        std::default_random_engine rndEngine((unsigned)time(nullptr));
        std::uniform_real_distribution<float> rndDist(-1.0f, 1.0f);

        // Initial particle positions on a circle
        std::vector<Particle> particles(particleCount);
        for (auto &particle : particles) {
            float x = rndDist(rndEngine);
            float y = rndDist(rndEngine);
            particle.position = glm::vec2(0, 0);
            particle.velocity = glm::vec2(rndDist(rndEngine) * 0.01, rndDist(rndEngine) * 0.01);
            particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }

        for (size_t i = 0; i < swapchain.GetImageCount(); i++)
            shaderStorageBuffers[i] = Buffer(
                sizeof(Particle) * particleCount,
                {BufferUsage::StorageBuffer, BufferUsage::VertexBuffer, BufferUsage::TransferDst}
            );
        vg::Allocate(shaderStorageBuffers, {MemoryProperty::HostVisible, MemoryProperty::HostCoherent});
        for (size_t i = 0; i < swapchain.GetImageCount(); i++)
            memcpy(shaderStorageBuffers[i].MapMemory(), particles.data(), sizeof(Particle) * particleCount);
    }

    Shader computeShader(ShaderStage::Compute, "resources/shaders/shader.comp.spv");
    ComputePipeline computePipeline(
        computeShader, PipelineLayout(
                           {{DescriptorSetLayoutBinding(0, DescriptorType::StorageBuffer, 1, ShaderStage::Compute),
                             DescriptorSetLayoutBinding(1, DescriptorType::StorageBuffer, 1, ShaderStage::Compute)}},
                           {}
                       )
    );

    DescriptorPool descriptorPool1(
        swapchain.GetImageCount(), {{DescriptorType::StorageBuffer, swapchain.GetImageCount() * 2}}
    );
    std::vector<vg::DescriptorSet> descriptorSets1;
    descriptorPool1.Allocate(
        std::vector<vg::DescriptorSetLayoutHandle>(
            swapchain.GetImageCount(), computePipeline.GetPipelineLayout().GetDescriptorSets()[0]
        ),
        &descriptorSets1
    );

    for (size_t i = 0; i < descriptorSets1.size(); i++) {
        descriptorSets1[i].AttachBuffer(
            DescriptorType::StorageBuffer, shaderStorageBuffers[(i - 1) % swapchain.GetImageCount()], 0,
            shaderStorageBuffers[i].GetSize(), 0, 0
        );
        descriptorSets1[i].AttachBuffer(
            DescriptorType::StorageBuffer, shaderStorageBuffers[i], 0, shaderStorageBuffers[i].GetSize(), 1, 0
        );
    }
    CmdBuffer computeCmdBuffer(computeQueue);
    QueryPool query(vg::QueryType::Timestamp, 2);
    SCOPED_DEVICE_CHANGE(&rendererDevice);
    Buffer particleBuffer(sizeof(Particle) * particleCount, {BufferUsage::VertexBuffer});
    vg::Allocate(&particleBuffer, {MemoryProperty::HostVisible, MemoryProperty::HostCoherent});

    std::vector<CmdBuffer> commandBuffer(swapchain.GetImageCount());
    std::vector<Semaphore> renderFinishedSemaphore(swapchain.GetImageCount()),
        imageAvailableSemaphore(swapchain.GetImageCount());
    std::vector<Fence> inFlightFence(swapchain.GetImageCount());
    for (int i = 0; i < swapchain.GetImageCount(); i++) {
        commandBuffer[i] = CmdBuffer(generalQueue);
        renderFinishedSemaphore[i] = Semaphore();
        imageAvailableSemaphore[i] = Semaphore();
        inFlightFence[i] = Fence(true);
    }

    UniformBufferObject ubo{glm::mat4(1.0f)};
    uint32_t currentFrame = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        inFlightFence[currentFrame].Await(true);

        // Swapchain resize.
        Swapchain oldSwapchain;
        if (recreateFramebuffer) {
            currentDevice->WaitUntilIdle();
            recreateFramebuffer = false;
            glfwGetFramebufferSize(window, &w, &h);
            std::swap(oldSwapchain, swapchain);
            swapchain = Swapchain(surface, 2, w, h, oldSwapchain);
            colorImage = Image(
                {swapchain.GetWidth(), swapchain.GetHeight()}, surface.GetFormat(),
                {ImageUsage::ColorAttachment, ImageUsage::TransientAttachment}, 1, 1, ImageTiling::Optimal,
                ImageLayout::Undefined, msaaSampleCount
            );
            depthImage = Image(
                {swapchain.GetWidth(), swapchain.GetHeight()}, depthImage.GetFormat(),
                {ImageUsage::DepthStencilAttachment}, 1, 1, ImageTiling::Optimal, ImageLayout::Undefined,
                msaaSampleCount
            );
            Allocate(Span<Image *const>{&depthImage, &colorImage}, {MemoryProperty::DeviceLocal});
            colorImageView = ImageView(colorImage, {ImageAspect::Color});
            depthImageView = ImageView(depthImage, {ImageAspect::Depth});
            for (int i = 0; i < swapchain.GetImageCount(); i++)
                swapChainFramebuffers[i] = Framebuffer(
                    renderPass, {colorImageView, depthImageView, swapchain.GetImageViews()[i]}, swapchain.GetWidth(),
                    swapchain.GetHeight()
                );
        }

        auto [imageIndex, result] = swapchain.GetNextImageIndex(imageAvailableSemaphore[currentFrame]);

        currentDevice = &computeDevice;
        computeCmdBuffer.Begin({})
            .Append(
                cmd::BindPipeline(computePipeline),
                cmd::BindDescriptorSets(
                    computePipeline.GetPipelineLayout(), PipelineBindPoint::Compute, 0, {descriptorSets1[imageIndex]}
                ),
                cmd::ResetQueryPool(query, 2), cmd::WriteTimestamp(PipelineStage::ComputeShader, query, 1),
                cmd::Dispatch(particleCount / 256, 1, 1), cmd::WriteTimestamp(PipelineStage::ComputeShader, query, 0)
            )
            .End()
            .Submit()
            .Await();
        auto [start, end] = query.GetResults<uint64_t, uint64_t>(2, 0, vg::QueryResult::_64Bit);

        currentDevice = &rendererDevice;
        memcpy(particleBuffer.MapMemory(), shaderStorageBuffers[imageIndex].MapMemory(), particleBuffer.GetSize());

        ubo.model = glm::rotate(ubo.model, glm::radians(0.3f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj =
            glm::perspective(glm::radians(45.0f), swapchain.GetWidth() / (float)swapchain.GetHeight(), 0.01f, 100.0f);
        ubo.proj[1][1] *= -1;
        memcpy(uniformBuffers.MapMemory() + imageIndex * sizeof(ubo), &ubo, sizeof(ubo));

        commandBuffer[currentFrame]
            .Clear()
            .Begin()
            .Append(
                cmd::BeginRenderpass(
                    renderPass, swapChainFramebuffers[imageIndex], {0, 0},
                    {swapchain.GetWidth(), swapchain.GetHeight()},
                    {ClearColor{0, 0, 0, 255}, ClearDepthStencil{1.0f, 0U}, ClearColor{0, 0, 0, 255}},
                    SubpassContents::Inline
                ),
                cmd::BindPipeline(renderPass.GetPipelines()[0]),
                cmd::BindDescriptorSets(
                    renderPass.GetPipelineLayouts()[0], PipelineBindPoint::Graphics, 0, {descriptorSets[imageIndex]}
                ),
                cmd::BindVertexBuffers(vertexBuffer, 0),
                cmd::BindIndexBuffer(vertexBuffer, sizeof(vertices[0]) * vertices.size(), IndexType::Uint16),
                cmd::SetViewport(Viewport(swapchain.GetWidth(), swapchain.GetHeight())),
                cmd::SetScissor(Scissor(swapchain.GetWidth(), swapchain.GetHeight())),
                cmd::PushConstants(renderPass.GetPipelineLayouts()[0], {ShaderStage::Vertex}, 0, glm::vec3(-2, 0, 0)),
                cmd::DrawIndexed(indices.size()),
                cmd::PushConstants(renderPass.GetPipelineLayouts()[0], {ShaderStage::Vertex}, 0, glm::vec3(0, 0, 0)),
                cmd::DrawIndexed(indices.size()), cmd::NextSubpass(SubpassContents::Inline),
                cmd::BindPipeline(renderPass.GetPipelines()[1]), cmd::BindVertexBuffers(particleBuffer, 0),
                cmd::Draw(particleCount), cmd::EndRenderpass()
            )
            .End()
            .Submit(
                {{PipelineStage::ColorAttachmentOutput, imageAvailableSemaphore[currentFrame]}},
                {renderFinishedSemaphore[currentFrame]}, inFlightFence[currentFrame]
            );
        generalQueue.Present({renderFinishedSemaphore[currentFrame]}, {swapchain}, {imageIndex});

        currentFrame = (currentFrame + 1) % swapchain.GetImageCount();
    }
    Fence::AwaitAll(inFlightFence);
    glfwTerminate();
}
