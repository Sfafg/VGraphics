#pragma once
#include "Handle.h"
#include "Device.h"
#include "Enums.h"
namespace vg
{
    class Shader
    {
    public:
        Shader(const Device& device, ShaderStage shaderStage, const char* path);

        Shader();
        Shader(Shader&& other) noexcept;
        Shader(const Shader& other) = delete;
        ~Shader();

        Shader& operator=(Shader&& other) noexcept;
        Shader& operator=(const Shader& other) = delete;


        ShaderStage GetStage() const;
        operator const ShaderHandle& () const;

#ifdef VULKAN_HPP
        operator vk::PipelineShaderStageCreateInfo() const;
#endif

    private:
        ShaderHandle m_handle;
        DeviceHandle m_device;
        ShaderStage m_stage;
    };
}