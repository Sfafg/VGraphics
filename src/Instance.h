#pragma once
#include <vector>
#include "Debug.h"
#include "Handle.h"

namespace vg
{
    class Instance
    {
    public:
        Instance();
        Instance(InstanceHandle handle);
        Instance(const std::vector<const char*>& requiredExtensions, Debug::Callback debugCallback = nullptr);

        Instance(Instance&& other) noexcept;
        Instance& operator=(Instance&& other) noexcept;
        Instance(const Instance& other) = delete;
        Instance& operator=(const Instance& other) = delete;

        ~Instance();

        operator InstanceHandle() const;

    private:
        InstanceHandle m_handle;
        DebugMessengerHandle m_debugMessenger;
        Debug::Callback m_debugCallback;
    };
    extern Instance instance;
}