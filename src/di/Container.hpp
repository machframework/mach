#pragma once

#include <format>
#include <functional>
#include <memory>
#include <stdexcept>
#include <typeindex>

#include "Scope.hpp"
#include "ServiceDescriptor.hpp"

namespace mach::detail::di
{
	class Container {
		
	public:
		template <typename T, typename... Deps>
		void addService(ServiceLifetime lifetime);

		const ServiceDescriptor& getDescriptor(std::type_index type) const;
        std::shared_ptr<void> getOrCreateSingleton(std::type_index type, Scope& container);

		Scope createScope();

	private:
		std::unordered_map<std::type_index, ServiceDescriptor> m_serviceRegistry;
        std::unordered_map<std::type_index, std::shared_ptr<void>> m_singletonInstances;
	};

    template <typename T, typename... Deps>
    void Container::addService(ServiceLifetime lifetime) {
        ServiceDescriptor descriptor{
            .type = typeid(T),
            .lifetime = lifetime,
            .factory = [](Scope& scope) {
                return std::make_shared<T>(
                    *scope.resolve<Deps>()...
                );
            }
        };

        auto [_, inserted] = m_serviceRegistry.emplace(
            descriptor.type,
            std::move(descriptor)
        );

        if (!inserted) {
            throw std::logic_error(
                std::format("Duplicate dependency registration: {}", descriptor.type.name())
            );
        }
    }
}
