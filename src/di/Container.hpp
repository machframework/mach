#pragma once

#include <functional>
#include <memory>
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
		Scope createScope() const;

	private:
		std::unordered_map<std::type_index, ServiceDescriptor> m_serviceRegistry;
	};

    template <typename T, typename... Deps>
    void Container::addService(ServiceLifetime lifetime) {
        ServiceDescriptor descriptor{
            .type = typeid(T),
            .lifetime = lifetime,
            .factory = [](const Scope& scope) {
                return std::make_shared<T>(
                    *scope.resolve<Deps>()...
                );
            }
        };

        m_serviceRegistry.emplace(
            descriptor.type,
            std::move(descriptor)
        );
    }
}
