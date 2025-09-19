#include <vulkan/vulkan.hpp>
#include "Instance.h"
#include "Device.h"
#include <vector>
#include <iostream>

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

bool checkValidationLayerSupport(const std::vector<const char *> &validationLayers) {
    for (const auto &layer : validationLayers) {
        bool contains = false;
        for (const auto &availableLayer : vk::enumerateInstanceLayerProperties()) {
            if (strcmp(layer, availableLayer.layerName) == 0) {
                contains = true;
                break;
            }
        }

        if (!contains) return false;
    }

    return true;
}
VkResult vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger
);
void vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator
);

namespace vg {
Instance *instance;
static VKAPI_ATTR VkBool32 VKAPI_CALL NativeDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData
) {
    if (pUserData != nullptr) {
        Instance::DebugCallback debugCallback = (Instance::DebugCallback)pUserData;
        debugCallback((MessageSeverity)messageSeverity, pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

Instance::Instance(
    Span<const char *const> requiredExtensions, DebugCallback debugCallback, bool enableValidationLayers
) {
    if (enableValidationLayers && !checkValidationLayerSupport(validationLayers))
        throw std::runtime_error("validation layers requested, but not available!");

    std::vector<const char *> extensions(requiredExtensions.begin(), requiredExtensions.end());
    if (enableValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    vk::ApplicationInfo appInfo("Hello Triangle", 1, "No Engine", 1, VK_API_VERSION_1_0);
    vk::InstanceCreateInfo createInfo({}, &appInfo, nullptr, extensions);

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        debugCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT(
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            NativeDebugCallback, (void *)debugCallback
        );
    } else {
        createInfo.enabledLayerCount = 0;
    }
    m_handle = vk::createInstance(createInfo);

    if (enableValidationLayers) {
        m_debugCallback = debugCallback;
        m_debugMessenger = m_handle.createDebugUtilsMessengerEXT(debugCreateInfo);
    }
}

Instance::Instance() : m_handle(nullptr), m_debugMessenger(nullptr), m_debugCallback(nullptr) {}

Instance::Instance(Instance &&other) noexcept : Instance() { *this = std::move(other); }

Instance::~Instance() {
    if (m_handle == nullptr) return;
    if (m_debugMessenger) m_handle.destroyDebugUtilsMessengerEXT(m_debugMessenger);
    m_handle.destroy();
    m_handle = nullptr;
}

Instance &Instance::operator=(Instance &&other) noexcept {
    if (&other == this) return *this;
    std::swap(m_handle, other.m_handle);
    std::swap(m_debugMessenger, other.m_debugMessenger);
    std::swap(m_debugCallback, other.m_debugCallback);

    return *this;
}

Instance::operator const InstanceHandle &() const { return m_handle; }
} // namespace vg

VkResult vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger
) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}
void vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator
) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) func(instance, debugMessenger, pAllocator);
}
