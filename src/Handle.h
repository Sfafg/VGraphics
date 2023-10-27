#pragma once
#include <iostream>

// TO DO: CHANGE TO TYPEDEF

#ifdef VULKAN_HPP
#define A(handleName, nativeName) handleName(const vk::nativeName& other) : m_handle(other) {} handleName& operator =(const vk::nativeName& rhs) { m_handle = rhs; return *this; } operator vk::nativeName() const { return vk::nativeName((Vk##nativeName) m_handle); }
#elif VULKAN_H_
#define A(handleName, nativeName) handleName(const Vk##nativeName& other) : m_handle(other) {}
#else 
#define A(handleName, nativeName)
#endif

#ifdef VULKAN_H_
#define B(handleName, nativeName) handleName& operator =(const Vk##nativeName& rhs) { m_handle = rhs; return *this; } operator Vk##nativeName() const { return (Vk##nativeName) m_handle; }
#else
#define B(handleName, nativeName)
#endif

#define HANDLE(InstanceHandle, Instance) class InstanceHandle { void* m_handle; public:InstanceHandle() :m_handle(nullptr) {}A(InstanceHandle, Instance)B(InstanceHandle, Instance) }

namespace vg
{
    HANDLE(InstanceHandle, Instance);
    HANDLE(DebugMessengerHandle, DebugUtilsMessengerEXT);
    HANDLE(DeviceHandle, Device);
    HANDLE(PhysicalDeviceHandle, PhysicalDevice);
    HANDLE(QueueHandle, Queue);
    HANDLE(SurfaceHandle, SurfaceKHR);
    HANDLE(SwapchainHandle, SwapchainKHR);
    HANDLE(ImageHandle, Image);
    HANDLE(ImageViewHandle, ImageView);
    HANDLE(ShaderHandle, ShaderModule);
    HANDLE(GraphicsPipelineHandle, Pipeline);
    HANDLE(PipelineLayoutHandle, PipelineLayout);
    HANDLE(RenderPassHandle, RenderPass);
    HANDLE(FramebufferHandle, Framebuffer);
    HANDLE(SemaphoreHandle, Semaphore);
    HANDLE(FenceHandle, Fence);
}

#undef A
#undef B
#undef HANDLE