#include "Window.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vg
{
    namespace Window
    {
        SurfaceHandle CreateWindowSurface(InstanceHandle instance, void* window)
        {
            SurfaceHandle surface;
            glfwCreateWindowSurface(*(VkInstance*) &instance, (GLFWwindow*) window, nullptr, (VkSurfaceKHR*) &surface);
            return surface;
        }
    }
}