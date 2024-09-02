#include <vulkan/vulkan.hpp>
#include "Sampler.h"
#include "Device.h"
namespace vg
{
    Sampler::Sampler(
        Filter magFilter,
        Filter minFilter,
        SamplerMipmapMode mipmapMode,
        SamplerAddressMode uAddressMode,
        SamplerAddressMode vAddressMode,
        SamplerAddressMode wAddressMode,
        float mipLodBias,
        float minLod,
        float maxLod,
        BorderColor borderColor,
        bool unnormalizedCoordinates)
    {
        m_handle = ((DeviceHandle) *currentDevice).createSampler({
            {},
            (vk::Filter) magFilter,
            (vk::Filter) minFilter,
            (vk::SamplerMipmapMode) mipmapMode,
            (vk::SamplerAddressMode) uAddressMode,
            (vk::SamplerAddressMode) vAddressMode,
            (vk::SamplerAddressMode) wAddressMode,
            mipLodBias,
            false,
            0,
            false,
            (vk::CompareOp) CompareOp::Never,
            minLod,
            maxLod,
            (vk::BorderColor) borderColor,
            unnormalizedCoordinates
            });
    }

    Sampler::Sampler(
        float maxAnisotropy,
        Filter magFilter,
        Filter minFilter,
        SamplerMipmapMode mipmapMode,
        SamplerAddressMode uAddressMode,
        SamplerAddressMode vAddressMode,
        SamplerAddressMode wAddressMode,
        float mipLodBias,
        float minLod,
        float maxLod,
        BorderColor borderColor,
        bool unnormalizedCoordinates)
    {
        m_handle = ((DeviceHandle) *currentDevice).createSampler({
            {},
            (vk::Filter) magFilter,
            (vk::Filter) minFilter,
            (vk::SamplerMipmapMode) mipmapMode,
            (vk::SamplerAddressMode) uAddressMode,
            (vk::SamplerAddressMode) vAddressMode,
            (vk::SamplerAddressMode) wAddressMode,
            mipLodBias,
            true,
            maxAnisotropy,
            false,
            (vk::CompareOp) CompareOp::Never,
            minLod,
            maxLod,
            (vk::BorderColor) borderColor,
            unnormalizedCoordinates
            });
    }

    Sampler::Sampler(
        vg::CompareOp compareOp,
        Filter magFilter,
        Filter minFilter,
        SamplerMipmapMode mipmapMode,
        SamplerAddressMode uAddressMode,
        SamplerAddressMode vAddressMode,
        SamplerAddressMode wAddressMode,
        float mipLodBias,
        float minLod,
        float maxLod,
        BorderColor borderColor,
        bool unnormalizedCoordinates)
    {
        m_handle = ((DeviceHandle) *currentDevice).createSampler({
            {},
            (vk::Filter) magFilter,
            (vk::Filter) minFilter,
            (vk::SamplerMipmapMode) mipmapMode,
            (vk::SamplerAddressMode) uAddressMode,
            (vk::SamplerAddressMode) vAddressMode,
            (vk::SamplerAddressMode) wAddressMode,
            mipLodBias,
            false,
            0,
            true,
            (vk::CompareOp) compareOp,
            minLod,
            maxLod,
            (vk::BorderColor) borderColor,
            unnormalizedCoordinates
            });
    }

    Sampler::Sampler(
        float maxAnisotropy,
        vg::CompareOp compareOp,
        Filter magFilter,
        Filter minFilter,
        SamplerMipmapMode mipmapMode,
        SamplerAddressMode uAddressMode,
        SamplerAddressMode vAddressMode,
        SamplerAddressMode wAddressMode,
        float mipLodBias,
        float minLod,
        float maxLod,
        BorderColor borderColor,
        bool unnormalizedCoordinates)
    {
        m_handle = ((DeviceHandle) *currentDevice).createSampler({
            {},
            (vk::Filter) magFilter,
            (vk::Filter) minFilter,
            (vk::SamplerMipmapMode) mipmapMode,
            (vk::SamplerAddressMode) uAddressMode,
            (vk::SamplerAddressMode) vAddressMode,
            (vk::SamplerAddressMode) wAddressMode,
            mipLodBias,
            true,
            maxAnisotropy,
            true,
            (vk::CompareOp) compareOp,
            minLod,
            maxLod,
            (vk::BorderColor) borderColor,
            unnormalizedCoordinates
            });
    }

    Sampler::Sampler() : m_handle(nullptr) {}

    Sampler::Sampler(Sampler&& other) noexcept
        :Sampler()
    {
        *this = std::move(other);
    }
    Sampler::~Sampler()
    {
        if (!m_handle)return;
        ((DeviceHandle) *currentDevice).destroySampler(m_handle);
        m_handle = nullptr;
    }

    Sampler& Sampler::operator=(Sampler&& other) noexcept
    {
        if (this == &other) return *this;

        std::swap(m_handle, other.m_handle);

        return *this;
    }
    Sampler::operator const SamplerHandle& () const
    {
        return m_handle;
    }

}