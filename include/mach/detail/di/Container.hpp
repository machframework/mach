#pragma once

#include <format>
#include <functional>
#include <memory>
#include <stdexcept>
#include <typeindex>

#include "Scope.hpp"
#include <mach/detail/di/ServiceLifetime.hpp>

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

        template <typename T>
        static constexpr bool isValidServiceType =
            std::is_class_v<T> &&
            !std::is_const_v<T> &&
            !std::is_reference_v<T> &&
            !std::is_pointer_v<T> &&
            !std::is_same_v<T, std::string> &&
            !std::is_same_v<T, std::string_view>;
	};

    template <typename T, typename... Deps>
    void Container::addService(ServiceLifetime lifetime) {
        static_assert(isValidServiceType<T>,
            "Service type must be a non-const, non-reference, non-pointer class type.");

        static_assert((isValidServiceType<Deps> && ...),
            "Dependency types must be non-const, non-reference, non-pointer class types.");

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
