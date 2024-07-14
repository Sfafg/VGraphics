#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
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
#include <fstream>
#include <string>
#include <sstream>

using namespace std::chrono_literals;
using namespace vg;
bool recreateFramebuffer = false;

struct MeshData
{
    vg::VertexBinding binding;
    std::vector <vg::VertexAttribute> attributes;
    std::vector<float> vertexData;
    std::vector<uint32_t> indexData;

    MeshData() :binding(0, 0)
    {}
};
MeshData ReadMeshData(const char* path)
{
    MeshData meshData;

    std::ifstream meshFile("mesh.txt");
    if (!meshFile.is_open())
    {
        std::cout << "Could not open mesh.txt";
        return meshData;
    }
    vg::VertexBinding vertexBinding(0, 0);
    std::vector<vg::VertexAttribute> attributes;
    std::vector<float> vertexData;
    std::vector<uint32_t> indexData;
    int vertexCount;
    int indexCount;

    std::string str;
    meshFile >> str >> vertexBinding.stride >> str;
    std::cout << vertexBinding.stride << '\n';

    while (!meshFile.eof())
    {
        vg::VertexAttribute attribute(0, 0, vg::Format::Undefined, 0);
        meshFile
            >> str >> attribute.location
            >> str >> attribute.binding
            >> str >> *(int*) &attribute.format
            >> str >> attribute.offset
            >> str;
        attributes.push_back(attribute);

        std::cout
            << attribute.location << " "
            << attribute.binding << " "
            << (int) attribute.format << " "
            << attribute.offset << '\n';

        if (str == "}")
            break;
        meshFile >> str;
    }
    meshFile >> str >> vertexCount >> str;
    std::cout << vertexCount << '\n';

    std::getline(meshFile, str);
    for (int i = 0; i < vertexCount; i++)
    {
        std::getline(meshFile, str);
        std::stringstream ss;
        ss << str;

        while (!ss.eof())
        {
            float a;
            ss >> a;

            vertexData.push_back(a);
            std::cout << vertexData[vertexData.size() - 1] << ", ";
        }
    }

    meshFile >> str >> str >> indexCount >> str;
    std::cout << indexCount << '\n';

    std::getline(meshFile, str);
    for (int i = 0; i < indexCount; i++)
    {
        std::getline(meshFile, str);
        std::stringstream ss;
        ss << str;

        while (!ss.eof())
        {
            int a;
            ss >> a;

            indexData.push_back(a);
            std::cout << indexData[indexData.size() - 1] << ", ";
        }
    }

    meshData.binding = vertexBinding;
    meshData.attributes = attributes;
    meshData.vertexData = vertexData;
    meshData.indexData = indexData;

    return meshData;
}

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VertexBinding& getBindingDescription()
    {
        static VertexBinding bindingDescription(0, sizeof(Vertex));

        return bindingDescription;
    }
    static std::array<VertexAttribute, 3>& getAttributeDescriptions()
    {
        static std::array<VertexAttribute, 3> attributeDescriptions = {
            VertexAttribute(0,0, Format::RGB32SFLOAT,offsetof(Vertex,position)),
            VertexAttribute(1,0, Format::RGB32SFLOAT,offsetof(Vertex,color)),
            VertexAttribute(2,0, Format::RG32SFLOAT,offsetof(Vertex,texCoord)),
        };

        return attributeDescriptions;
    }
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

int main()
{
    //TO DO: CmdBuffer usage
    //TO DO: CmdBuffer inheritance
    //TO DO: GraphicsPipeline inheritance

    MeshData mesh = ReadMeshData("mesh.txt");

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(1600, 900, "Vulkan", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) {recreateFramebuffer = true; });

    vg::instance = Instance({ "VK_KHR_surface", "VK_KHR_win32_surface" },
        [](Debug::Severity severity, const char* message) {
            if (severity < Debug::Severity::Warning) return;
            std::cout << message << '\n' << '\n';
        });

    SurfaceHandle windowSurface = Window::CreateWindowSurface(vg::instance, window);
    vg::currentDevice = Device({ QueueType::Graphics, QueueType::Present, QueueType::Transfer }, { "VK_KHR_swapchain" }, windowSurface,
        [](auto supportedQueues, auto supportedExtensions, auto type, DeviceLimits limits) {
            return type == DeviceType::Integrated;
        });

    Surface surface(windowSurface, Format::BGRA8SRGB, ColorSpace::SRGBNL);

    int w, h; glfwGetFramebufferSize(window, &w, &h);
    Shader vertexShader(ShaderStage::Vertex, "C:/Projekty/Vulkan/VGraphics2/src/shaders/shaderVert.spv");
    Shader fragmentShader(ShaderStage::Fragment, "C:/Projekty/Vulkan/VGraphics2/src/shaders/shaderFrag.spv");
    RenderPass renderPass(
        {
            Attachment(surface.GetFormat(), ImageLayout::PresentSrc),
            Attachment(Format::D32SFLOAT, ImageLayout::DepthStencilAttachmentOptimal,ImageLayout::Undefined,nullptr)
        },
        {
            Subpass(
                GraphicsPipeline{
                    {&vertexShader, &fragmentShader},
                    VertexLayout(1U,&Vertex::getBindingDescription(),Vertex::getAttributeDescriptions().size(),Vertex::getAttributeDescriptions().data()),
                    InputAssembly(),
                    Tesselation(),
                    ViewportState(Viewport(w, h), Scissor(w, h)),
                    Rasterizer(true, false, PolygonMode::Fill, CullMode::Back, FrontFace::CounterClockwise, DepthBias(), 10.0f),
                    Multisampling(),
                    DepthStencil(true,true,CompareOp::Less),
                    ColorBlending(true, LogicOp::Copy, { 0,0,0,0 }),
                    { DynamicState::Viewport, DynamicState::Scissor },
                    {{0, DescriptorType::UniformBuffer, 1, ShaderStage::Vertex},
                     {1, DescriptorType::CombinedImageSampler, 1, ShaderStage::Fragment}}
                },
                {},
                {AttachmentReference(0, ImageLayout::ColorAttachmentOptimal)},
                {},
                AttachmentReference(1, ImageLayout::DepthStencilAttachmentOptimal)
            )
        },
        {}
    );

    Swapchain swapchain(surface, 2, w, h);

    Image depthImage(swapchain.GetWidth(), swapchain.GetHeight(), { Format::D32SFLOAT,Format::D32SFLOATS8UINT,Format::x8D24UNORMPACK }, { FormatFeature::DepthStencilAttachment }, { ImageUsage::DepthStencilAttachment });
    Allocate(&depthImage, { MemoryProperty::DeviceLocal });
    ImageView depthImageView(depthImage, { ImageAspect::Depth });

    std::vector<Framebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(swapchain.GetImageCount());
    for (int i = 0; i < swapchain.GetImageCount(); i++)
        swapChainFramebuffers[i] = Framebuffer(renderPass, { swapchain.GetImageViews()[i],depthImageView }, swapchain.GetWidth(), swapchain.GetHeight());

    // Allocate buffer in DeviceLocal memory.
    Buffer vertexBuffer(sizeof(mesh.vertexData[0]) * mesh.vertexData.size() + sizeof(mesh.indexData[0]) * mesh.indexData.size(), { vg::BufferUsage::VertexBuffer,vg::BufferUsage::IndexBuffer, vg::BufferUsage::TransferDst });
    vg::Allocate({ &vertexBuffer }, { MemoryProperty::DeviceLocal });
    {
        // Allocate staging buffer in HostVisible memory.
        Buffer stagingBuffer(vertexBuffer.GetSize(), vg::BufferUsage::TransferSrc);
        vg::Allocate({ &stagingBuffer }, { MemoryProperty::HostVisible });
        char* data = (char*) stagingBuffer.MapMemory();
        memcpy(data, mesh.vertexData.data(), sizeof(mesh.vertexData[0]) * mesh.vertexData.size());
        memcpy(data + sizeof(mesh.vertexData[0]) * mesh.vertexData.size(), mesh.indexData.data(), sizeof(mesh.indexData[0]) * mesh.indexData.size());
        stagingBuffer.UnmapMemory();

        // Copy staging data to vertex buffer
        vg::CmdBuffer(currentDevice.transferQueue).Append(
            cmd::CopyBuffer(stagingBuffer, vertexBuffer, { BufferCopyRegion {vertexBuffer.GetSize() } })
        ).Submit().Await();
    }

    Buffer uniformBuffers(sizeof(UniformBufferObject) * swapchain.GetImageCount(), BufferUsage::UniformBuffer);
    vg::Allocate(&uniformBuffers, { MemoryProperty::HostVisible, MemoryProperty::HostCoherent });
    char* uniformBufferMemory = (char*) uniformBuffers.MapMemory();

    /// Load Image.
    int texWidth, texHeight, texChannels;
    unsigned char* pixels = stbi_load("C:/Projekty/Vulkan/VGraphics2/src/textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    Image texImage(texWidth, texHeight, Format::RGBA8SRGB, { ImageUsage::TransferDst , ImageUsage::Sampled });
    vg::Allocate(&texImage, { MemoryProperty::DeviceLocal });
    {
        // Upload data to GPU memory
        Buffer texStagingBuffer(texWidth * texHeight * 4, { BufferUsage::TransferSrc });
        vg::Allocate(&texStagingBuffer, { MemoryProperty::HostVisible, MemoryProperty::HostCoherent });
        memcpy(texStagingBuffer.MapMemory(), pixels, texStagingBuffer.GetSize());
        stbi_image_free(pixels);

        vg::CmdBuffer(vg::currentDevice.transferQueue).Append(
            cmd::PipelineBarier(PipelineStage::TopOfPipe, PipelineStage::Transfer, Dependency::None, { {Access::None, Access::TransferWrite, ImageLayout::TransferDstOptimal, texImage, ImageAspect::Color} }),
            cmd::CopyBufferToImage(texStagingBuffer, texImage, ImageLayout::TransferDstOptimal, { BufferImageCopyRegion(0,{ImageAspect::Color},texWidth,texHeight,1) }),
            cmd::PipelineBarier(PipelineStage::Transfer, PipelineStage::FragmentShader, Dependency::None, { {Access::TransferWrite, Access::ShaderRead, ImageLayout::TransferDstOptimal, ImageLayout::ShaderReadOnlyOptimal, texImage, ImageAspect::Color} })
        ).Submit().Await();
    }
    ImageView imageView(texImage, { ImageAspect::Color });
    Sampler sampler(currentDevice.GetProperties().limits.maxSamplerAnisotropy, Filter::Linear, Filter::Linear);

    // Create descriptor pools
    DescriptorPool descriptorPool(swapchain.GetImageCount(), {
        {DescriptorType::UniformBuffer, swapchain.GetImageCount()},
        {DescriptorType::CombinedImageSampler, swapchain.GetImageCount()}
        });

    // Create and allocate descriptor set layouts.
    std::vector<vg::DescriptorSetLayoutHandle> layouts(swapchain.GetImageCount(), renderPass.GetPipelineLayouts()[0].GetDescriptorSets()[0]);
    auto descriptorSets = descriptorPool.Allocate(layouts);

    for (size_t i = 0; i < descriptorSets.size(); i++)
    {
        descriptorSets[i].AttachBuffer(uniformBuffers, sizeof(UniformBufferObject) * i, sizeof(UniformBufferObject), 0, 0);
        descriptorSets[i].AttachImage(ImageLayout::ShaderReadOnlyOptimal, imageView, sampler, 1, 0);
    }

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
            depthImage = Image(w, h, Format::D32SFLOAT, { ImageUsage::DepthStencilAttachment });
            Allocate(&depthImage, { MemoryProperty::DeviceLocal });
            depthImageView = ImageView(depthImage, { ImageAspect::Depth });

            std::swap(oldSwapchain, swapchain);
            swapchain = Swapchain(surface, 2, w, h, Usage::ColorAttachment, PresentMode::Fifo, CompositeAlpha::Opaque, oldSwapchain);
            for (int i = 0; i < swapchain.GetImageCount(); i++)
                swapChainFramebuffers[i] = Framebuffer(renderPass, { swapchain.GetImageViews()[i],depthImageView }, swapchain.GetWidth(), swapchain.GetHeight());

        }

        uint32_t imageIndex = swapchain.GetNextImageIndex(imageAvailableSemaphore);

        // Update UniformBuffer.
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), swapchain.GetWidth() / (float) swapchain.GetHeight(), 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;
        memcpy(uniformBufferMemory + imageIndex * sizeof(ubo), &ubo, sizeof(ubo));

        commandBuffer.Append(
            cmd::BeginRenderpass(renderPass, swapChainFramebuffers[imageIndex], 0, 0, swapchain.GetWidth(), swapchain.GetHeight(), 0.01, 0.01, 0.01, 1),
            cmd::BindPipeline(renderPass.GetPipelines()[0]),
            cmd::BindVertexBuffer(vertexBuffer, 0),
            cmd::BindIndexBuffer(vertexBuffer, sizeof(mesh.vertexData[0]) * mesh.vertexData.size(), (int) vk::IndexType::eUint32),
            cmd::SetViewport(Viewport(swapchain.GetWidth(), swapchain.GetHeight())),
            cmd::SetScissor(Scissor(swapchain.GetWidth(), swapchain.GetHeight())),
            cmd::BindDescriptorSets(renderPass.GetPipelineLayouts()[0], 0, { descriptorSets[imageIndex] }),
            cmd::DrawIndexed(mesh.indexData.size()),
            cmd::EndRenderpass()
        ).Submit({ {{ {PipelineStage::ColorAttachmentOutput, imageAvailableSemaphore} },{ renderFinishedSemaphore }} }, inFlightFence);
        currentDevice.presentQueue.Present({ renderFinishedSemaphore }, { swapchain }, { imageIndex });
    }
    Fence::AwaitAll({ inFlightFence });
    glfwTerminate();
}