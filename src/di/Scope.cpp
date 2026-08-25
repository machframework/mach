#include <mach/detail/di/Scope.hpp>

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>

#include "ResolutionGuard.hpp"
#include <mach/detail/di/Container.hpp>

namespace
{
    std::string buildMissingRegistrationMessage(
        std::type_index missingType,
        const std::vector<std::type_index>& resolutionStack) {
        if (resolutionStack.size() <= 1) {
            return std::format("Mach DI error: service is not registered: {}", missingType.name());
        }

        const std::type_index parentType = resolutionStack[resolutionStack.size() - 2];

        return std::format(
            "Mach DI error: failed to resolve service '{}': "
            "dependency '{}' is not registered",
            parentType.name(),
            missingType.name());
    }
}

namespace mach::detail::di
{
    std::shared_ptr<void> Scope::resolve(std::type_index type) {
        ResolutionGuard guard(m_resolutionStack, type);

        if (m_scopedInstances.contains(type)) {
            return m_scopedInstances.find(type)->second;
        }

        const auto* descriptor = m_container.getDescriptor(type);

        if (descriptor == nullptr) {
            throw std::logic_error(buildMissingRegistrationMessage(type, m_resolutionStack));
        }

        if (descriptor->lifetime == ServiceLifetime::Singleton) {
            return m_container.getOrCreateSingleton(descriptor->type, *this);
        }

        auto instance = descriptor->factory(*this);

        if (descriptor->lifetime == ServiceLifetime::Scoped) {
            m_scopedInstances.emplace(descriptor->type, instance);
        } else {
            m_transientInstances.push_back(instance);
        }

        return instance;
    }

    Scope::Scope(Container& container) : m_container(container) {}
}
