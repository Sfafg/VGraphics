#pragma once
#include "Handle.h"

namespace vg {
namespace Debug {

bool SetName(vg::BufferHandle buffer, const char *name);
bool SetName(vg::ImageHandle image, const char *name);
bool SetName(vg::DeviceHandle device, const char *name);
bool SetName(vg::SemaphoreHandle semaphore, const char *name);
bool SetName(vg::DeviceMemoryHandle memory, const char *name);
bool SetName(vg::RenderPassHandle renderPass, const char *name);
bool SetName(vg::SamplerHandle sampler, const char *name);
bool SetName(vg::SwapchainHandle swapchain, const char *name);

} // namespace Debug
} // namespace vg
