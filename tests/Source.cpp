#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vulkan/vulkan.hpp>
#include "Instance.h"
#include "Device.h"
#include "Surface.h"
#include "Swapchain.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include <iostream>

#include <chrono>
class Timer
{
    static std::vector<Timer*> timers;
private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;

public:
    Timer(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now())
    {
        timers.push_back(this);
    }

    static void End(const std::string& name, std::ostream& os = std::cout)
    {
        for (int i = timers.size() - 1; i >= 0; i--)
        {
            if (timers[i]->name == name)
            {
                os << *timers[i];
                timers.erase(timers.begin() + i);
                return;
            }
        }

    }

    friend std::ostream& operator<<(std::ostream& os, const Timer& t);
};
std::vector<Timer*> Timer::timers;
std::ostream& operator<<(std::ostream& os, const Timer& t)
{
    auto d = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - t.start);
    os << "Timer: " << t.name << ": " << d << '\n';
    return os;
}

// Swap
// Clear

void recordCommandBuffer(const vg::RenderPass& renderPass, const std::vector<vk::Framebuffer>& framebuffers, const vg::Swapchain& swapchain, VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = (vg::RenderPassHandle) renderPass;
    renderPassInfo.framebuffer = framebuffers[imageIndex];

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = { swapchain.GetWidth(), swapchain.GetHeight() };

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;


    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, (vg::GraphicsPipelineHandle) renderPass.m_graphicsPipelines[0]);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = swapchain.GetWidth();
    viewport.height = swapchain.GetHeight();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = { swapchain.GetWidth(), swapchain.GetHeight() };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

int main()
{
    // GLFW and window initialization.
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);

    // Instance Creation/
    vg::instance = vg::Instance({ "VK_KHR_surface", "VK_KHR_win32_surface" },
        [](vg::Debug::Severity severity, const char* message) {
            if (severity < vg::Debug::Severity::Warning) return;
            std::cout << message << '\n' << '\n';
        });

    // Device picking.
    vg::Surface surface; glfwCreateWindowSurface((vg::InstanceHandle) vg::instance, window, nullptr, (VkSurfaceKHR*) &surface);
    vg::Device device({ vg::Queue::Type::Graphics, vg::Queue::Type::Present }, { "VK_KHR_swapchain" }, surface);

    // Render pass and graphics pipeline creation.
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    vg::Swapchain swapchain(surface, device, 2, w, h);

    vg::Shader vertexShader(device, vg::ShaderStage::Vertex, "C:/Projekty/Vulkan/VGraphics2/src/shaders/vert.spv");
    vg::Shader fragmentShader(device, vg::ShaderStage::Fragment, "C:/Projekty/Vulkan/VGraphics2/src/shaders/frag.spv");

    vg::GraphicsPipeline graphicsPipeline(
        { &vertexShader, &fragmentShader },
        vg::VertexLayout(),
        vg::InputAssembly(),
        vg::Tesselation(),
        vg::ViewportState(vg::Viewport(w, h), vg::Scissor(w, h)),
        vg::Rasterizer(false, false, vg::PolygonMode::Fill, vg::CullMode::Back, vg::FrontFace::Clockwise, vg::DepthBias(), 1.0f),
        vg::Multisampling(),
        vg::DepthStencil(),
        vg::ColorBlending(false, vg::LogicOp::Copy, { 0,0,0,0 }),
        { vg::DynamicState::Viewport, vg::DynamicState::Scissor },
        vg::PipelineLayout()
    );

    vg::RenderPass renderPass(
        device,
        { vg::Attachment(swapchain.GetFormat(), vg::ImageLayout::PresentSrc) },
        std::vector<vg::Subpass>{vg::Subpass(graphicsPipeline, {}, { vg::AttachmentReference(0, vg::ImageLayout::ColorAttachmentOptimal) }, {}, nullptr)},
        std::vector<vg::SubpassDependency>{}
    );

    std::vector<vg::Framebuffer> swapChainFramebuffers;
    swapChainFramebuffers.resize(swapchain.GetImageViews().size());
    for (size_t i = 0; i < swapchain.GetImageViews().size(); i++)
    {
        swapChainFramebuffers[i] = vg::Framebuffer(device, renderPass, { swapchain.GetImageViews()[i] }, swapchain.GetWidth(), swapchain.GetHeight(), 1);
    }

    VkCommandPool commandPool;
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = device.graphicsQueue.GetIndex();
    if (vkCreateCommandPool((vg::DeviceHandle) device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers((vg::DeviceHandle) device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (vkCreateSemaphore((vg::DeviceHandle) device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore((vg::DeviceHandle) device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence((vg::DeviceHandle) device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create semaphores!");
    }
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        vkWaitForFences((vg::DeviceHandle) device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences((vg::DeviceHandle) device, 1, &inFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR((vg::DeviceHandle) device, (vg::SwapchainHandle) swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        vkResetCommandBuffer(commandBuffer, 0);
        std::vector<vk::Framebuffer> _swapChainFramebuffers;
        for (const auto& framebuffer : swapChainFramebuffers) _swapChainFramebuffers.push_back((vg::FramebufferHandle) framebuffer);
        recordCommandBuffer(renderPass, _swapChainFramebuffers, swapchain, commandBuffer, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        if (vkQueueSubmit((vg::QueueHandle) device.graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = { (vg::SwapchainHandle) swapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        vkQueuePresentKHR((vg::QueueHandle) device.presentQueue, &presentInfo);
    }

    vkDestroySemaphore((vg::DeviceHandle) device, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore((vg::DeviceHandle) device, renderFinishedSemaphore, nullptr);
    vkDestroyFence((vg::DeviceHandle) device, inFlightFence, nullptr);
    vkDestroyCommandPool((vg::DeviceHandle) device, commandPool, nullptr);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
    glfwTerminate();
}