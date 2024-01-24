#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "Instance.h"
#include "Device.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "Synchronization.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
using namespace vg;

bool framebufferResize = true;
SurfaceHandle CreateWindowSurface(InstanceHandle, GLFWwindow*);
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h) { framebufferResize = true; });

    vg::instance = Instance({ "VK_KHR_surface", "VK_KHR_win32_surface" },
        [](Debug::Severity severity, const char* message) {
            if (severity < Debug::Severity::Warning) return;
            std::cout << message << '\n' << '\n';
        });

    SurfaceHandle windowSurface = CreateWindowSurface(vg::instance, window);
    Device device({ Queue::Type::Graphics, Queue::Type::Present }, { "VK_KHR_swapchain", "VK_NV_fill_rectangle" }, (SurfaceHandle) windowSurface);
    Surface surface(device, windowSurface, Format::B8G8R8A8SRGB, ColorSpace::SRGBNL);

    int w, h; glfwGetFramebufferSize(window, &w, &h);
    Shader vertexShader(device, ShaderStage::Vertex, "C:/Projekty/Vulkan/VGraphics2/src/shaders/shaderVert.spv");
    Shader fragmentShader(device, ShaderStage::Fragment, "C:/Projekty/Vulkan/VGraphics2/src/shaders/shaderFrag.spv");
    RenderPass renderPass(
        device,
        {
            Attachment(surface.GetFormat(), ImageLayout::PresentSrc)
        },
        {
            Subpass(
                vg::GraphicsPipeline{
                    {&vertexShader, &fragmentShader},
                    VertexLayout(),
                    InputAssembly(),
                    Tesselation(),
                    ViewportState(Viewport(w, h), Scissor(w, h)),
                    Rasterizer(false, false, PolygonMode::Fill, CullMode::Back, FrontFace::Clockwise, DepthBias(), 1.0f),
                    Multisampling(),
                    DepthStencil(),
                    ColorBlending(false, LogicOp::Copy, { 0,0,0,0 }),
                    { DynamicState::Viewport, DynamicState::Scissor },
                    PipelineLayout()
                },
                {},
                { AttachmentReference(0, ImageLayout::ColorAttachmentOptimal) }
            )
        },
        {}
    );

    Swapchain swapchain(surface, device, 2, w, h);
    std::vector<Framebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(swapchain.GetImageViews().size());
    for (size_t i = 0; i < swapchain.GetImageViews().size(); i++)
        swapChainFramebuffers[i] = Framebuffer(device, renderPass, { swapchain.GetImageViews()[i] }, swapchain.GetWidth(), swapchain.GetHeight(), 1);

    CommandBuffer commandBuffer(device.graphicsQueue);
    Semaphore renderFinishedSemaphore(device);
    Semaphore imageAvailableSemaphore(device);
    Fence inFlightFence(device, true);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        WaitForFences(device, { inFlightFence });
        ResetFences(device, { inFlightFence });

        Swapchain oldSwapchain;
        glfwGetFramebufferSize(window, &w, &h);
        if (swapchain.GetHeight() != h || swapchain.GetWidth() != w)
        {
            std::swap(swapchain, oldSwapchain);
            swapchain = Swapchain(surface, device, 2, w, h, Usage::ColorAttachment, PresentMode::Fifo, CompositeAlpha::Opaque, oldSwapchain);
            for (size_t i = 0; i < swapchain.GetImageViews().size(); i++)
                swapChainFramebuffers[i] = Framebuffer(device, renderPass, { swapchain.GetImageViews()[i] }, swapchain.GetWidth(), swapchain.GetHeight(), 1);
            framebufferResize = false;
        }

        uint32_t imageIndex = swapchain.GetNextImageIndex(UINT64_MAX, imageAvailableSemaphore, Fence());

        commandBuffer.Clear();
        commandBuffer.Begin();
        commandBuffer.Append(cmd::BeginRenderpass(renderPass, swapChainFramebuffers[imageIndex], 0, 0, swapchain.GetWidth(), swapchain.GetHeight()));
        commandBuffer.Append(cmd::BindPipeline(renderPass.m_graphicsPipelines[0]));
        commandBuffer.Append(cmd::SetViewport(Viewport(swapchain.GetWidth(), swapchain.GetHeight())));
        commandBuffer.Append(cmd::SetScissor(Scissor(swapchain.GetWidth(), swapchain.GetHeight())));
        commandBuffer.Append(cmd::Draw(3, 1));
        commandBuffer.Append(cmd::EndRenderpass());
        commandBuffer.End();

        std::vector<SubmitInfo> submits = { {{ WaitStage(PipelineStage::ColorAttachmentOutput, imageAvailableSemaphore) },{ commandBuffer },{ renderFinishedSemaphore }} };
        device.graphicsQueue.Submit(submits, inFlightFence);
        device.presentQueue.Present({ renderFinishedSemaphore }, { swapchain }, { imageIndex });
    }
    WaitForFences(device, { inFlightFence });
    glfwTerminate();
}
SurfaceHandle CreateWindowSurface(InstanceHandle instance, GLFWwindow* window) { SurfaceHandle surface; glfwCreateWindowSurface(*(VkInstance*) &instance, window, nullptr, (VkSurfaceKHR*) &surface); return surface; }