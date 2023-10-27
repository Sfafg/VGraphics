#pragma once

namespace vg
{
    class Instance;
    namespace Debug
    {
        enum class Severity
        {
            Verbose = 1,
            Info = 16,
            Warning = 256,
            Error = 4096
        };

        typedef void(*Callback)(Severity, const char* message);
    }

}