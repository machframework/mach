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
        constexpr bool serviceIsClass =
            std::is_class_v<T>;

        constexpr bool serviceIsNotPointer =
            !std::is_pointer_v<T>;

        constexpr bool serviceIsNotReference =
            !std::is_reference_v<T>;

        constexpr bool serviceIsNotCvQualified =
            !std::is_const_v<T> &&
            !std::is_volatile_v<T>;

        constexpr bool serviceIsAllowedType =
            !std::same_as<T, std::string> &&
            !std::same_as<T, std::string_view>;

        constexpr bool dependenciesAreClasses =
            (std::is_class_v<Deps> && ...);

        constexpr bool dependenciesAreNotPointers =
            (!std::is_pointer_v<Deps> && ...);

        constexpr bool dependenciesAreNotReferences =
            (!std::is_reference_v<Deps> && ...);

        constexpr bool dependenciesAreNotCvQualified =
            ((!std::is_const_v<Deps> &&
                !std::is_volatile_v<Deps>) && ...);

        constexpr bool dependenciesAreAllowedTypes =
            ((!std::same_as<Deps, std::string> &&
                !std::same_as<Deps, std::string_view>) && ...);


        // Service shape validation

        static_assert(
            serviceIsNotPointer,
            "Mach DI error: service type must not be a pointer"
            );

        if constexpr (serviceIsNotPointer) {
            static_assert(
                serviceIsNotReference,
                "Mach DI error: service type must not be a reference"
                );
        }

        if constexpr (
            serviceIsNotPointer &&
            serviceIsNotReference
            ) {
            static_assert(
                serviceIsNotCvQualified,
                "Mach DI error: service type must not be const or volatile"
                );
        }

        if constexpr (
            serviceIsNotPointer &&
            serviceIsNotReference &&
            serviceIsNotCvQualified
            ) {
            static_assert(
                serviceIsClass,
                "Mach DI error: service type must be a class"
                );
        }

        if constexpr (
            serviceIsNotPointer &&
            serviceIsNotReference &&
            serviceIsNotCvQualified &&
            serviceIsClass
            ) {
            static_assert(
                serviceIsAllowedType,
                "Mach DI error: std::string and std::string_view cannot be registered as services"
                );
        }


        // Dependency shape validation

        static_assert(
            dependenciesAreNotPointers,
            "Mach DI error: dependency types must not be pointers"
            );

        if constexpr (dependenciesAreNotPointers) {
            static_assert(
                dependenciesAreNotReferences,
                "Mach DI error: dependency types must not be references"
                );
        }

        if constexpr (
            dependenciesAreNotPointers &&
            dependenciesAreNotReferences
            ) {
            static_assert(
                dependenciesAreNotCvQualified,
                "Mach DI error: dependency types must not be const or volatile"
                );
        }

        if constexpr (
            dependenciesAreNotPointers &&
            dependenciesAreNotReferences &&
            dependenciesAreNotCvQualified
            ) {
            static_assert(
                dependenciesAreClasses,
                "Mach DI error: dependency types must be classes"
                );
        }

        if constexpr (
            dependenciesAreNotPointers &&
            dependenciesAreNotReferences &&
            dependenciesAreNotCvQualified &&
            dependenciesAreClasses
            ) {
            static_assert(
                dependenciesAreAllowedTypes,
                "Mach DI error: std::string and std::string_view cannot be registered as dependencies"
                );
        }

        constexpr bool validService =
            serviceIsClass &&
            serviceIsNotPointer &&
            serviceIsNotReference &&
            serviceIsNotCvQualified &&
            serviceIsAllowedType;

        constexpr bool validDependencies =
            dependenciesAreClasses &&
            dependenciesAreNotPointers &&
            dependenciesAreNotReferences &&
            dependenciesAreNotCvQualified &&
            dependenciesAreAllowedTypes;

        if constexpr (validService && validDependencies) {
            constexpr bool completeService =
                isCompleteType<T>;

            constexpr bool completeDependencies =
                (isCompleteType<Deps> && ...);

            static_assert(
                completeService,
                "Mach DI error: service type must be complete before registration"
                );

            static_assert(
                completeDependencies,
                "Mach DI error: dependency types must be complete before registration"
                );

            if constexpr (completeService && completeDependencies) {
                constexpr bool concreteService =
                    !std::is_abstract_v<T>;

                constexpr bool concreteDependencies =
                    (!std::is_abstract_v<Deps> && ...);

                static_assert(
                    concreteService,
                    "Mach DI error: service type cannot be abstract"
                    );

                static_assert(
                    concreteDependencies,
                    "Mach DI error: dependency types cannot be abstract"
                    );

                if constexpr (concreteService && concreteDependencies) {
                    constexpr bool destructibleService =
                        std::is_destructible_v<T>;

                    constexpr bool destructibleDependencies =
                        (std::is_destructible_v<Deps> && ...);

                    static_assert(
                        destructibleService,
                        "Mach DI error: service type must be destructible"
                        );

                    static_assert(
                        destructibleDependencies,
                        "Mach DI error: dependency types must be destructible"
                        );

                    if constexpr (
                        destructibleService &&
                        destructibleDependencies
                        ) {
                        constexpr bool noSelfDependency =
                            (!std::same_as<T, Deps> && ...);

                        constexpr bool uniqueDependencies =
                            areUniqueTypes<Deps...>;

                        static_assert(
                            noSelfDependency,
                            "Mach DI error: a service cannot directly depend on itself"
                            );

                        static_assert(
                            uniqueDependencies,
                            "Mach DI error: dependency types must be unique"
                            );

                        if constexpr (
                            noSelfDependency &&
                            uniqueDependencies
                            ) {
                            constexpr bool constructibleImplementation =
                                std::is_constructible_v<T, Deps&...>;

                            static_assert(
                                constructibleImplementation,
                                "Mach DI error: service cannot be constructed from the declared dependencies"
                                );

                            if constexpr (constructibleImplementation) {
								const std::type_index type = typeid(T);

                                ServiceDescriptor descriptor{
                                    .type = type,
                                    .lifetime = lifetime,
                                    .factory = [](Scope& scope) {
                                        return std::make_shared<T>(
                                            scope.resolve<Deps>()...
                                        );
                                    }
                                };

                                auto [_, inserted] = m_serviceRegistry.emplace(
                                    type,
                                    std::move(descriptor)
                                );

                                if (!inserted) {
                                    throw std::logic_error(
                                        std::format(
                                            "Mach DI error: duplicate service registration: {}",
                                            type.name()
                                        )
                                    );
                                }
                            }
                        }
                    }
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
