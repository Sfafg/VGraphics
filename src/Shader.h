#pragma once
#include "Handle.h"
#include "Device.h"
#include "VulkanEnums.h"
namespace vg
{
    class Shader
    {
    public:
        Shader();
        Shader(const Device& device, ShaderStage shaderStage, const char* path);

        Shader(Shader&& other) noexcept;
        Shader& operator=(Shader&& other) noexcept;
        Shader& operator=(const Shader& other) = delete;
        Shader(const Shader& other) = delete;

        ~Shader();

        ShaderStage GetStage() const;
        operator ShaderHandle() const;

#ifdef VULKAN_HPP
        operator vk::PipelineShaderStageCreateInfo() const;
#endif
        
    private:
        ShaderHandle m_handle;
        DeviceHandle m_device;
        ShaderStage m_stage;
    };
}