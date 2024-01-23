#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include "Instance.h"
#include "Device.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "Fence.h"
#include "MiniBenchmark.h"
#include <iostream>
#include <math.h>
#include <fstream>
using namespace vg;

void INIT()
{
    // GLFW and window initialization.
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);

    {
        // Instance Creation.
        vg::instance = Instance({ "VK_KHR_surface", "VK_KHR_win32_surface" },
            [](Debug::Severity severity, const char* message) {
                if (severity < Debug::Severity::Warning) return;
                std::cout << message << '\n' << '\n';
            });

        // Device picking.
        Surface surface; glfwCreateWindowSurface((InstanceHandle) instance, window, nullptr, (VkSurfaceKHR*) &surface);
        Device device({ Queue::Type::Graphics, Queue::Type::Present }, { "VK_KHR_swapchain", "VK_NV_fill_rectangle" }, (SurfaceHandle) surface);

        // Render pass and graphics pipeline creation.
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        Swapchain swapchain((SurfaceHandle) surface, device, 2, w, h);

        Shader vertexShader(device, ShaderStage::Vertex, "C:/Projekty/Vulkan/VGraphics2/src/shaders/vert.spv");
        Shader fragmentShader(device, ShaderStage::Fragment, "C:/Projekty/Vulkan/VGraphics2/src/shaders/frag.spv");

        GraphicsPipeline graphicsPipeline(
            { &vertexShader, &fragmentShader },
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
        );

        RenderPass renderPass(
            device,
            { Attachment(swapchain.GetFormat(), ImageLayout::PresentSrc) },
            std::vector<Subpass>{Subpass(graphicsPipeline, {}, { AttachmentReference(0, ImageLayout::ColorAttachmentOptimal) }, {}, nullptr)},
            std::vector<SubpassDependency>{}
        );

        std::vector<Framebuffer> swapChainFramebuffers;
        swapChainFramebuffers.resize(swapchain.GetImageViews().size());
        for (size_t i = 0; i < swapchain.GetImageViews().size(); i++)
            swapChainFramebuffers[i] = Framebuffer(device, renderPass, { swapchain.GetImageViews()[i] }, swapchain.GetWidth(), swapchain.GetHeight(), 1);

        // Command buffers and synchronization.
        CommandBuffer commandBuffer(device.graphicsQueue);
        Semaphore renderFinishedSemaphore(device);
        Semaphore imageAvailableSemaphore(device);
        Fence inFlightFence(device, true);

    }
    glfwTerminate();
}
int main()
{
    MiniBenchmark::SetHightPriority();
    auto samples = MiniBenchmark::Benchmark(INIT, 100);
    std::ofstream unprocessed("C:/Projekty/Vulkan/VGraphics2/tests/Benchmarks/unprocessed.txt");
    for (int i = 0; i < samples.values.size(); i++)
    {
        unprocessed << samples.values[i] << "\n";
    }
    while (true)
    {
        float average = samples.Average();
        float deviation = 0;
        for (int i = 0; i < samples.values.size(); i++)
        {
            deviation += pow(samples.values[i] - average, 2);
        }
        deviation = sqrt(deviation / (samples.values.size() - 1));

        bool removedAtLeasOne = false;
        for (int i = samples.values.size() - 1; i >= 0; i--)
        {
            if (samples.values[i] - average > deviation * 3)
            {
                samples.values[i] = samples.values[samples.values.size() - 1];
                samples.values.pop_back();
                removedAtLeasOne = true;
            }
        }
        if (!removedAtLeasOne) break;
    }

    std::ofstream processed("C:/Projekty/Vulkan/VGraphics2/tests/Benchmarks/processed.txt");
    for (int i = 0; i < samples.values.size(); i++)
    {
        processed << samples.values[i] << "\n";
    }

    return 0;
}