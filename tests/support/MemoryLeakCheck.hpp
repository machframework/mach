#pragma once

#ifdef _WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

namespace testing
{
    class MemoryLeakCheck {

    public:
        MemoryLeakCheck() {

#ifdef _WIN32
#ifdef _DEBUG
            _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#endif
        }

        MemoryLeakCheck(const MemoryLeakCheck&) = delete;
        MemoryLeakCheck& operator=(const MemoryLeakCheck&) = delete;

        MemoryLeakCheck(MemoryLeakCheck&&) = delete;
        MemoryLeakCheck& operator=(MemoryLeakCheck&&) = delete;

        ~MemoryLeakCheck() = default;
    };
}
