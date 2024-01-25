#pragma once
#include <vector>
#include "Debug.h"
#include "Handle.h"

namespace vg
{

    /**
     *@brief Vulkan Instance and Error handling
     *
     * Handles instance and will automatically turns on validation layers in debug mode.
     * There should be only one Instance created before any API calls.
     * Make sure to assign static variable of vg::instance.
     *
     */
    class Instance
    {
    public:
        /**
         *@brief Construct a new Instance object
         *
         * @param requiredExtensions list of required extensions
         * @param debugCallback optional callback to custom logging function
         */
        Instance(const std::vector<const char*>& requiredExtensions, Debug::Callback debugCallback = nullptr);

        Instance();
        Instance(Instance&& other) noexcept;
        Instance(const Instance& other) = delete;
        ~Instance();

        Instance& operator= (Instance&& other) noexcept;
        Instance& operator=(const Instance& other) = delete;

        operator const InstanceHandle& () const;

    private:
        InstanceHandle m_handle;
        DebugMessengerHandle m_debugMessenger;
        Debug::Callback m_debugCallback;
    };
    extern Instance instance;
}