#include <vulkan/vulkan.hpp>
#include "NamedResources.h"
#include "Instance.h"
#include "Device.h"

VkResult vkSetDebugUtilsObjectNameEXT(VkDevice, const VkDebugUtilsObjectNameInfoEXT *);

bool vg::Debug::SetName(vg::BufferHandle obj, const char *name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo(vk::ObjectType::eBuffer, *(uint64_t *)(&obj), name);
    vk::Result r = ((vk::Device *)vg::currentDevice)->setDebugUtilsObjectNameEXT(&nameInfo);
    if (r != vk::Result::eSuccess) return false;
    return true;
}

bool vg::Debug::SetName(vg::ImageHandle obj, const char *name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo(vk::ObjectType::eImage, *(uint64_t *)(&obj), name);
    vk::Result r = ((vk::Device *)vg::currentDevice)->setDebugUtilsObjectNameEXT(&nameInfo);
    if (r != vk::Result::eSuccess) return false;
    return true;
}

bool SetName(vg::DeviceHandle obj, const char *name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo(vk::ObjectType::eDevice, *(uint64_t *)(&obj), name);
    vk::Result r = ((vk::Device *)vg::currentDevice)->setDebugUtilsObjectNameEXT(&nameInfo);
    if (r != vk::Result::eSuccess) return false;
    return true;
}

bool SetName(vg::SemaphoreHandle obj, const char *name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo(vk::ObjectType::eSemaphore, *(uint64_t *)(&obj), name);
    vk::Result r = ((vk::Device *)vg::currentDevice)->setDebugUtilsObjectNameEXT(&nameInfo);
    if (r != vk::Result::eSuccess) return false;
    return true;
}

bool SetName(vg::DeviceMemoryHandle obj, const char *name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo(vk::ObjectType::eDeviceMemory, *(uint64_t *)(&obj), name);
    vk::Result r = ((vk::Device *)vg::currentDevice)->setDebugUtilsObjectNameEXT(&nameInfo);
    if (r != vk::Result::eSuccess) return false;
    return true;
}

bool SetName(vg::RenderPassHandle obj, const char *name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo(vk::ObjectType::eRenderPass, *(uint64_t *)(&obj), name);
    vk::Result r = ((vk::Device *)vg::currentDevice)->setDebugUtilsObjectNameEXT(&nameInfo);
    if (r != vk::Result::eSuccess) return false;
    return true;
}

bool SetName(vg::SamplerHandle obj, const char *name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo(vk::ObjectType::eSampler, *(uint64_t *)(&obj), name);
    vk::Result r = ((vk::Device *)vg::currentDevice)->setDebugUtilsObjectNameEXT(&nameInfo);
    if (r != vk::Result::eSuccess) return false;
    return true;
}

bool SetName(vg::SwapchainHandle obj, const char *name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo(vk::ObjectType::eSwapchainKHR, *(uint64_t *)(&obj), name);
    vk::Result r = ((vk::Device *)vg::currentDevice)->setDebugUtilsObjectNameEXT(&nameInfo);
    if (r != vk::Result::eSuccess) return false;
    return true;
}

VkResult vkSetDebugUtilsObjectNameEXT(VkDevice device, const VkDebugUtilsObjectNameInfoEXT *info) {
    auto func = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(
        *(VkInstance *)vg::instance, "vkSetDebugUtilsObjectNameEXT"
    );
    if (func != nullptr) return func(device, info);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}
