#include <mach/detail/di/Container.hpp>

#include <mach/detail/di/ServiceDescriptor.hpp>

namespace mach::detail::di
{
    const ServiceDescriptor* Container::getDescriptor(std::type_index type) const {
        const auto descriptor = m_serviceRegistry.find(type);

        if (descriptor == m_serviceRegistry.end()) {
            return nullptr;
        }

        return &descriptor->second;
    }

    Scope Container::createScope() {
        return Scope(*this);
    }

    void Container::finalizeRegistrations() {
        for (const auto& [type, descriptor] : m_serviceRegistry) {
            if (descriptor.access == ServiceAccess::User) {
                if (const auto internalDependency = findInaccessibleDependency(descriptor)) {
                    throw std::logic_error(
                        "Mach error: service '" + std::string(type.name()) +
                        "' cannot depend directly or indirectly on an internal Mach service '" +
                        std::string(internalDependency->name()) + "'");
                }
            }
            if (descriptor.lifetime == ServiceLifetime::Singleton) {
                if (const auto scopedDependency = findScopedDependency(descriptor)) {
                    throw std::logic_error(
                        "Mach error: singleton service '" + std::string(type.name()) +
                        "' cannot depend directly or indirectly on scoped service '" +
                        std::string(scopedDependency->name()) + "'");
                }

                m_singletonEntries.try_emplace(type, std::make_unique<SingletonEntry>());
            }
        }
    }

    std::shared_ptr<void> Container::getOrCreateSingleton(std::type_index type, Scope& scope)
        const {
        auto& entry = *m_singletonEntries.at(type);
        const auto& descriptor = m_serviceRegistry.at(type);

        std::call_once(entry.initializationFlag, [&] {
            entry.instance = descriptor.factory(scope);
        });

        return entry.instance;
    }

    std::optional<std::type_index> Container::findScopedDependency(
        const ServiceDescriptor& descriptor) const {
        for (const auto dependencyType : descriptor.dependencies) {
            const auto& dependency = m_serviceRegistry.at(dependencyType);

            if (dependency.lifetime == ServiceLifetime::Scoped) {
                return dependencyType;
            }
            if (const auto scopedDependency = findScopedDependency(dependency)) {
                return scopedDependency;
            }
        }

        return std::nullopt;
    }

    std::optional<std::type_index> Container::findInaccessibleDependency(
        const ServiceDescriptor& descriptor) const {
        for (const auto dependencyType : descriptor.dependencies) {
            const auto dependencyIt = m_serviceRegistry.find(dependencyType);
            if (dependencyIt == m_serviceRegistry.end()) {
                throw std::logic_error(
                    "Mach error: service '" + std::string(descriptor.type.name()) +
                    "' depends on unregistered service '" + std::string(dependencyType.name()) +
                    "'");
            }

            const auto& dependency = dependencyIt->second;

            if (dependency.access == ServiceAccess::Internal) {
                return dependencyType;
            }
            if (const auto scopedDependency = findInaccessibleDependency(dependency)) {
                return scopedDependency;
            }
        }

        return std::nullopt;
    }
}
