#pragma once

#include <format>
#include <functional>
#include <memory>
#include <stdexcept>
#include <typeindex>

#include <mach/detail/di/ServiceLifetime.hpp>
#include <mach/detail/di/ServiceTraits.hpp>

#include "Scope.hpp"

namespace mach::detail::di
{
	class Container {
		
	public:
		template <typename T, typename... Deps>
		void addService(ServiceLifetime lifetime);

		template <typename T>
        void addSingletonInstance(T&& instance);

		const ServiceDescriptor& getDescriptor(std::type_index type) const;
        std::shared_ptr<void> getOrCreateSingleton(std::type_index type, Scope& container);

		Scope createScope();

	private:
		std::unordered_map<std::type_index, ServiceDescriptor> m_serviceRegistry;
        std::unordered_map<std::type_index, std::shared_ptr<void>> m_singletonInstances;
	};

    template <typename T, typename... Deps>
    void Container::addService(ServiceLifetime lifetime) {
        ///
        constexpr bool validService = isValidServiceType<T>;
        constexpr bool validDependencies = (isValidServiceType<Deps> && ...);

        static_assert(
            validService,
            "Mach DI error: service must be a complete, non-cv-qualified class type"
            );

        static_assert(
            validDependencies,
            "Mach DI error: every dependency must be a complete, non-cv-qualified class type"
            );

        if constexpr (validService && validDependencies) {
            constexpr bool concreteService =
                !std::is_abstract_v<T>;

            constexpr bool concreteDependencies =
                (!std::is_abstract_v<Deps> && ...);

            constexpr bool destructibleService =
                std::is_destructible_v<T>;

            constexpr bool destructibleDependencies =
                (std::is_destructible_v<Deps> && ...);

            constexpr bool noSelfDependency =
                (!std::same_as<T, Deps> && ...);

            constexpr bool uniqueDependencies =
                areUniqueTypes<Deps...>;

            constexpr bool constructibleImplementation =
                std::is_constructible_v<T, Deps&...>;

            static_assert(
                concreteService,
                "Mach DI error: service type cannot be abstract"
                );

            static_assert(
                concreteDependencies,
                "Mach DI error: dependency types cannot be abstract"
                );

            static_assert(
                destructibleService,
                "Mach DI error: service type must be destructible"
                );

            static_assert(
                destructibleDependencies,
                "Mach DI error: dependency types must be destructible"
                );

            static_assert(
                noSelfDependency,
                "Mach DI error: a service cannot directly depend on itself"
                );

            static_assert(
                uniqueDependencies,
                "Mach DI error: dependency types must be unique"
                );

            static_assert(
                constructibleImplementation,
                "Mach DI error: service cannot be constructed from the declared dependencies"
                );

            constexpr bool validRegistration =
                concreteService &&
                concreteDependencies &&
                destructibleService &&
                destructibleDependencies &&
                noSelfDependency &&
                uniqueDependencies &&
                constructibleImplementation;

            if constexpr (validRegistration) {
                ServiceDescriptor descriptor{
                    .type = typeid(T),
                    .lifetime = lifetime,
                    .factory = [](Scope& scope) {
                        return std::make_shared<T>(
                            scope.resolve<Deps>()...
                        );
                    }
                };

                auto [_, inserted] = m_serviceRegistry.emplace(
                    descriptor.type,
                    std::move(descriptor)
                );

                if (!inserted) {
                    throw std::logic_error(
                        std::format("Mach DI error: duplicate dependency registration: {}", descriptor.type.name())
                    );
                }
            }
        }
    }

    template <typename T>
    void Container::addSingletonInstance(T&& instance) {
        ServiceDescriptor descriptor{
            .type = typeid(T),
            .lifetime = ServiceLifetime::Singleton
        };

        m_singletonInstances.emplace(
            typeid(T),
            std::make_shared<T>(std::move(instance))
        );

        m_serviceRegistry.emplace(
            descriptor.type,
            std::move(descriptor)
        );
    }
}
