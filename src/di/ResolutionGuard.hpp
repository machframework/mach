#pragma once

#include <vector>
#include <typeindex>

namespace mach::detail::di
{
    class ResolutionGuard {
    public:
        ResolutionGuard(std::vector<std::type_index>& resolutionStack, std::type_index type)
            : resolutionStack_(resolutionStack) {
            resolutionStack_.push_back(type);
        }

        ~ResolutionGuard() {
            resolutionStack_.pop_back();
        }

        ResolutionGuard(const ResolutionGuard&) = delete;
        ResolutionGuard& operator=(const ResolutionGuard&) = delete;

    private:
        std::vector<std::type_index>& resolutionStack_;
    };
}
