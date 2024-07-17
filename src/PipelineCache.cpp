#include <vulkan/vulkan.hpp>
#include "PipelineCache.h"
#include "Device.h"

namespace vg
{
    PipelineCache::PipelineCache(const void* initialData, uint32_t initialDataSize, bool isExternallySynchronized)
    {
        m_handle = ((DeviceHandle) currentDevice).createPipelineCache(vk::PipelineCacheCreateInfo(
            (vk::PipelineCacheCreateFlags) isExternallySynchronized,
            initialDataSize, initialData
        ));
    }

    PipelineCache::PipelineCache() :m_handle(nullptr)
    {}

    PipelineCache::PipelineCache(PipelineCache&& other) noexcept
    {
        std::swap(m_handle, other.m_handle);
    }

    PipelineCache::~PipelineCache()
    {
        if (m_handle == nullptr) return;

        ((DeviceHandle) currentDevice).destroyPipelineCache(m_handle);

        m_handle = nullptr;
    }

    PipelineCache& PipelineCache::operator=(PipelineCache&& other) noexcept
    {
        if (&other == this)
            return *this;

        std::swap(m_handle, other.m_handle);

        return *this;
    }

    PipelineCache::operator const PipelineCacheHandle& () const
    {
        return m_handle;
    }

    void* PipelineCache::GetData(uint32_t* dataSize) const
    {

        auto result = ((DeviceHandle) currentDevice).getPipelineCacheData(m_handle, (size_t*) dataSize, nullptr);
        void* data = new char[*dataSize];
        auto result2 = ((DeviceHandle) currentDevice).getPipelineCacheData(m_handle, (size_t*) dataSize, data);

        return data;
    }

    void PipelineCache::MergeCaches(const std::vector<PipelineCacheHandle>& caches)
    {
        ((DeviceHandle) currentDevice).mergePipelineCaches(m_handle, *(std::vector<vk::PipelineCache>*) & caches);
    }
}