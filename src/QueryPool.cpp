#include <vulkan/vulkan.hpp>
#include "QueryPool.h"

namespace vg
{
    QueryPool::QueryPool(QueryType type, uint32_t queryCount)
    {
        m_handle = ((DeviceHandle) *vg::currentDevice).createQueryPool({ {}, (vk::QueryType) type, queryCount });
    }

    QueryPool::QueryPool() :m_handle(nullptr) {}
    QueryPool::QueryPool(QueryPool&& other) noexcept
        :m_handle(nullptr)
    {
        *this = std::move(other);
    }
    QueryPool::~QueryPool()
    {
        ((DeviceHandle) *vg::currentDevice).destroyQueryPool(m_handle);
    }

    QueryPool& QueryPool::operator=(QueryPool&& other) noexcept
    {
        if (this == &other)
            return *this;

        std::swap(m_handle, other.m_handle);

        return *this;
    }
    QueryPool::operator const QueryPoolHandle& () const
    {
        return m_handle;
    }

    void QueryPool::GetResults(int queryCount, int firstQuery, void* data, uint32_t dataSize, uint32_t stride, Flags<QueryResult> resultFlags)
    {
        vkGetQueryPoolResults((DeviceHandle) *vg::currentDevice, m_handle, firstQuery, queryCount, dataSize, data, stride, resultFlags);
    }
}