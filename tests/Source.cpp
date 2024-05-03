#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Instance.h"
#include "Device.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "Synchronization.h"
#include "Flags.h"
#include "Buffer.h"
#include "MemoryManager.h"
#include "Window.h"
using namespace std::chrono_literals;
using namespace vg;
bool recreateFramebuffer = false;

struct Vertex
{
    struct Vector2 { float x, y; } position;
    struct Vector3 { float r, g, b; } color;

    static vk::VertexInputBindingDescription& getBindingDescription()
    {
        static vk::VertexInputBindingDescription bindingDescription(0, sizeof(Vertex));

        return bindingDescription;
    }
    static std::array<vk::VertexInputAttributeDescription, 2>& getAttributeDescriptions()
    {
        static std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions = {
            vk::VertexInputAttributeDescription(0,0,(vk::Format) Format::RG32SFLOAT,offsetof(Vertex,position)),
            vk::VertexInputAttributeDescription(1,0,(vk::Format) Format::RGB32SFLOAT,offsetof(Vertex,color))
        };

        return attributeDescriptions;
    }
};
std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};
std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(1600, 900, "Vulkan", nullptr, nullptr);
    glfwSetWindowPos(window, -1920 / 2 - 1600 / 2, 1080 / 2 - 900 / 2);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) {recreateFramebuffer = true; });

    vg::instance = Instance({ "VK_KHR_surface", "VK_KHR_win32_surface" },
        [](Debug::Severity severity, const char* message) {
            if (severity < Debug::Severity::Warning) return;
            std::cout << message << '\n' << '\n';
        });

    SurfaceHandle windowSurface = Window::CreateWindowSurface(vg::instance, window);
    Device device({ Queue::Type::Graphics, Queue::Type::Present, Queue::Type::Transfer }, { "VK_KHR_swapchain" }, windowSurface,
        [](auto supportedQueues, auto supportedExtensions, auto type, Limits limits) {
            return type == Device::Type::Integrated;
        });
    Surface surface(device, windowSurface, Format::BGRA8SRGB, ColorSpace::SRGBNL);

    int w, h; glfwGetFramebufferSize(window, &w, &h);
    Shader vertexShader(device, ShaderStage::Vertex, "C:/Projekty/Vulkan/VGraphics2/src/shaders/shaderVert.spv");
    Shader fragmentShader(device, ShaderStage::Fragment, "C:/Projekty/Vulkan/VGraphics2/src/shaders/shaderFrag.spv");

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout((DeviceHandle) device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    VkPipelineLayout layout;
    vkCreatePipelineLayout((DeviceHandle) device, &pipelineLayoutInfo, nullptr, &layout);

    RenderPass renderPass(
        device,
        {
            Attachment(surface.GetFormat(), ImageLayout::PresentSrc)
        },
        {
            Subpass(
                vg::GraphicsPipeline{
                    {&vertexShader, &fragmentShader},
                    VertexLayout(1U,&Vertex::getBindingDescription(),Vertex::getAttributeDescriptions().size(),Vertex::getAttributeDescriptions().data()),
                    InputAssembly(),
                    Tesselation(),
                    ViewportState(Viewport(w, h), Scissor(w, h)),
                    Rasterizer(true, false, PolygonMode::Fill, CullMode::Back, FrontFace::Clockwise, DepthBias(), 10.0f),
                    Multisampling(),
                    DepthStencil(),
                    ColorBlending(true, LogicOp::Copy, { 0,0,0,0 }),
                    { DynamicState::Viewport, DynamicState::Scissor },
                    PipelineLayout((PipelineLayoutHandle) layout)
                },
                {},
                { AttachmentReference(0, ImageLayout::ColorAttachmentOptimal) }
            )
        },
        {}
    );
    Swapchain swapchain(surface, device, 2, w, h);
    std::vector<Framebuffer> swapChainFramebuffers = swapchain.CreateFramebuffers(renderPass);

    std::vector<Buffer> uniformBuffers;
    std::vector<void*> buffersMappedMemory;
    uniformBuffers.resize(swapchain.GetImageViews().size());
    buffersMappedMemory.resize(uniformBuffers.size());
    for (int i = 0; i < uniformBuffers.size(); i++)
    {
        uniformBuffers[i] = Buffer(device, sizeof(UniformBufferObject), { BufferUsage::UniformBuffer });
        vg::Allocate(device, &uniformBuffers[i], { { MemoryProperty::HostVisible, MemoryProperty::HostCoherent } });
        buffersMappedMemory[i] = uniformBuffers[i].MapMemory();
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(swapchain.GetImageViews().size());
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(swapchain.GetImageViews().size());
    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool((DeviceHandle) device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    std::vector<VkDescriptorSetLayout> layouts(swapchain.GetImageViews().size(), descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchain.GetImageViews().size());
    allocInfo.pSetLayouts = layouts.data();
    std::vector<VkDescriptorSet> descriptorSets;
    descriptorSets.resize(swapchain.GetImageViews().size());
    if (vkAllocateDescriptorSets((DeviceHandle) device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    for (size_t i = 0; i < swapchain.GetImageViews().size(); i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = (BufferHandle) uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        vkUpdateDescriptorSets((DeviceHandle) device, 1, &descriptorWrite, 0, nullptr);
    }

    // Allocate buffer in DeviceLocal memory.
    Buffer vertexBuffer(device, sizeof(vertices[0]) * vertices.size() + sizeof(indices[0]) * indices.size(), { vg::BufferUsage::VertexBuffer,vg::BufferUsage::IndexBuffer, vg::BufferUsage::TransferDst });
    vg::Allocate(device, { &vertexBuffer }, { MemoryProperty::DeviceLocal });
    {
        // Allocate staging buffer in HostVisible memory.
        Buffer stagingBuffer(device, vertexBuffer.GetSize(), vg::BufferUsage::TransferSrc);
        vg::Allocate(device, { &stagingBuffer }, { MemoryProperty::HostVisible });
        char* data = (char*) stagingBuffer.MapMemory();
        memcpy(data, vertices.data(), sizeof(vertices[0]) * vertices.size());
        memcpy(data + sizeof(vertices[0]) * vertices.size(), indices.data(), sizeof(indices[0]) * indices.size());
        stagingBuffer.UnmapMemory();

        // Copy staging data to vertex buffer
        vg::CommandBuffer transfer(device.transferQueue);
        transfer.Append(cmd::CopyBuffer(stagingBuffer, vertexBuffer, { CopyRegion {vertexBuffer.GetSize() } }));
        Fence copyFence = transfer.Submit({ vg::SubmitInfo({}, {transfer},{}) });
        Fence::AwaitAll({ copyFence });
    }

    CommandBuffer commandBuffer(device.graphicsQueue);
    Semaphore renderFinishedSemaphore(device);
    Semaphore imageAvailableSemaphore(device);
    Fence inFlightFence(device, true);
    auto startTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        glfwFocusWindow(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        Fence::AwaitAll({ inFlightFence }, true);

        // Swapchain resize.
        Swapchain oldSwapchain;
        if (recreateFramebuffer)
        {
            std::swap(oldSwapchain, swapchain);
            swapchain = Swapchain(surface, device, 2, w, h, Usage::ColorAttachment, PresentMode::Fifo, CompositeAlpha::Opaque, oldSwapchain);
            swapChainFramebuffers = swapchain.CreateFramebuffers(renderPass);
        }

        uint32_t imageIndex = swapchain.GetNextImageIndex(imageAvailableSemaphore);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        UniformBufferObject ubo{};

        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(90.0f), swapchain.GetWidth() / (float) swapchain.GetHeight(), 0.1f, 10.0f);
        memcpy(buffersMappedMemory[imageIndex], &ubo, sizeof(ubo));

        commandBuffer.Append(
            cmd::BeginRenderpass(renderPass, swapChainFramebuffers[imageIndex], 0, 0, swapchain.GetWidth(), swapchain.GetHeight()),
            cmd::BindPipeline(renderPass.m_graphicsPipelines[0]),
            cmd::BindVertexBuffer(vertexBuffer, 0),
            cmd::BindIndexBuffer(vertexBuffer, sizeof(vertices[0]) * vertices.size(), (int) vk::IndexType::eUint16),
            cmd::SetViewport(Viewport(swapchain.GetWidth(), swapchain.GetHeight())),
            cmd::SetScissor(Scissor(swapchain.GetWidth(), swapchain.GetHeight())),
            cmd::BindDescriptorSets((PipelineLayoutHandle) layout, 0, { descriptorSets[imageIndex] }),
            cmd::DrawIndexed(indices.size()),
            cmd::EndRenderpass()
        );

        commandBuffer.Submit({ {{ {PipelineStage::ColorAttachmentOutput, imageAvailableSemaphore} },{ commandBuffer },{ renderFinishedSemaphore }} }, inFlightFence);
        device.presentQueue.Present({ renderFinishedSemaphore }, { swapchain }, { imageIndex });
    }
    Fence::AwaitAll({ inFlightFence });
    vkDestroyDescriptorSetLayout((DeviceHandle) device, descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool((DeviceHandle) device, descriptorPool, nullptr);
    glfwTerminate();
}