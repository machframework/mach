#pragma once

#include <format>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

#include <mach/detail/di/InternalServiceTypes.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>
#include <mach/detail/di/ServiceTraits.hpp>

#include "Scope.hpp"
#include "ServiceDescriptor.hpp"

namespace mach::detail::di
{
    class Container {

    public:
        template <typename T, typename... Deps>
        void addService(ServiceLifetime lifetime, ServiceAccess access = ServiceAccess::User);

        template <typename T>
        T& addSingletonInstance(T&& instance, ServiceAccess access = ServiceAccess::Internal);

        const ServiceDescriptor* getDescriptor(std::type_index type) const;
        std::shared_ptr<void> getOrCreateSingleton(std::type_index type, Scope& scope) const;

        [[nodiscard]] Scope createScope();

        void finalizeRegistrations();

        template <typename T>
        void reserveInternal();

    private:
        std::optional<std::type_index> findScopedDependency(
            const ServiceDescriptor& descriptor) const;

        std::optional<std::type_index> findInaccessibleDependency(
            const ServiceDescriptor& descriptor) const;

        std::unordered_map<std::type_index, ServiceDescriptor> m_serviceRegistry;
        std::unordered_set<std::type_index> m_reservedTypes;

        struct SingletonEntry {
            std::once_flag initializationFlag;
            std::shared_ptr<void> instance;
        };

        std::unordered_map<std::type_index, std::unique_ptr<SingletonEntry>> m_singletonEntries;
    };

    template <typename T, typename... Deps>
    void Container::addService(ServiceLifetime lifetime, ServiceAccess access) {
        constexpr bool serviceIsClass = std::is_class_v<T>;

        constexpr bool serviceIsNotPointer = !std::is_pointer_v<T>;

        constexpr bool serviceIsNotReference = !std::is_reference_v<T>;

        constexpr bool serviceIsNotCvQualified = !std::is_const_v<T> && !std::is_volatile_v<T>;

        constexpr bool serviceIsAllowedType =
            !std::same_as<T, std::string> && !std::same_as<T, std::string_view>;

        constexpr bool dependenciesAreClasses = (std::is_class_v<Deps> && ...);

        constexpr bool dependenciesAreNotPointers = (!std::is_pointer_v<Deps> && ...);

        constexpr bool dependenciesAreNotReferences = (!std::is_reference_v<Deps> && ...);

        constexpr bool dependenciesAreNotCvQualified =
            ((!std::is_const_v<Deps> && !std::is_volatile_v<Deps>) && ...);

        constexpr bool dependenciesAreAllowedTypes =
            ((!std::same_as<Deps, std::string> && !std::same_as<Deps, std::string_view>) && ...);

        // Service shape validation
        static_assert(serviceIsNotPointer, "Mach DI error: service type must not be a pointer");

        if constexpr (serviceIsNotPointer) {
            static_assert(
                serviceIsNotReference,
                "Mach DI error: service type must not be a reference");
        }

        if constexpr (serviceIsNotPointer && serviceIsNotReference) {
            static_assert(
                serviceIsNotCvQualified,
                "Mach DI error: service type must not be const or volatile");
        }

        if constexpr (serviceIsNotPointer && serviceIsNotReference && serviceIsNotCvQualified) {
            static_assert(serviceIsClass, "Mach DI error: service type must be a class");
        }

        if constexpr (
            serviceIsNotPointer && serviceIsNotReference && serviceIsNotCvQualified &&
            serviceIsClass
        ) {
            static_assert(
                serviceIsAllowedType,
                "Mach DI error: std::string and std::string_view cannot be registered as services");
        }

        // Dependency shape validation
        static_assert(
            dependenciesAreNotPointers,
            "Mach DI error: dependency types must not be pointers");

        if constexpr (dependenciesAreNotPointers) {
            static_assert(
                dependenciesAreNotReferences,
                "Mach DI error: dependency types must not be references");
        }

        if constexpr (dependenciesAreNotPointers && dependenciesAreNotReferences) {
            static_assert(
                dependenciesAreNotCvQualified,
                "Mach DI error: dependency types must not be const or volatile");
        }

        if constexpr (
            dependenciesAreNotPointers && dependenciesAreNotReferences &&
            dependenciesAreNotCvQualified
        ) {
            static_assert(
                dependenciesAreClasses,
                "Mach DI error: dependency types must be classes");
        }

        if constexpr (
            dependenciesAreNotPointers && dependenciesAreNotReferences &&
            dependenciesAreNotCvQualified && dependenciesAreClasses
        ) {
            static_assert(
                dependenciesAreAllowedTypes,
                "Mach DI error: std::string and std::string_view cannot be registered as "
                "dependencies");
        }

        constexpr bool validService = serviceIsClass && serviceIsNotPointer &&
                                      serviceIsNotReference && serviceIsNotCvQualified &&
                                      serviceIsAllowedType;

        constexpr bool validDependencies =
            dependenciesAreClasses && dependenciesAreNotPointers && dependenciesAreNotReferences &&
            dependenciesAreNotCvQualified && dependenciesAreAllowedTypes;

        if constexpr (validService && validDependencies) {
            constexpr bool completeService = isCompleteType<T>;

            constexpr bool completeDependencies = (isCompleteType<Deps> && ...);

            static_assert(
                completeService,
                "Mach DI error: service type must be complete before registration");

            static_assert(
                completeDependencies,
                "Mach DI error: dependency types must be complete before registration");

            if constexpr (completeService && completeDependencies) {
                constexpr bool concreteService = !std::is_abstract_v<T>;

                constexpr bool concreteDependencies = (!std::is_abstract_v<Deps> && ...);

                static_assert(concreteService, "Mach DI error: service type cannot be abstract");

                static_assert(
                    concreteDependencies,
                    "Mach DI error: dependency types cannot be abstract");

                if constexpr (concreteService && concreteDependencies) {
                    constexpr bool destructibleService = std::is_destructible_v<T>;

                    constexpr bool destructibleDependencies = (std::is_destructible_v<Deps> && ...);

                    static_assert(
                        destructibleService,
                        "Mach DI error: service type must be destructible");

                    static_assert(
                        destructibleDependencies,
                        "Mach DI error: dependency types must be destructible");

                    if constexpr (destructibleService && destructibleDependencies) {
                        constexpr bool noSelfDependency = (!std::same_as<T, Deps> && ...);

                        constexpr bool uniqueDependencies = areUniqueTypes<Deps...>;

                        static_assert(
                            noSelfDependency,
                            "Mach DI error: a service cannot directly depend on itself");

                        static_assert(
                            uniqueDependencies,
                            "Mach DI error: dependency types must be unique");

                        if constexpr (noSelfDependency && uniqueDependencies) {
                            constexpr bool serviceAllowed = !isForbiddenDIType<T>;

                            constexpr bool dependenciesAllowed = (!isForbiddenDIType<Deps> && ...);

                            static_assert(
                                serviceAllowed,
                                "Mach DI error: this Mach framework type cannot be registered as a "
                                "service");

                            static_assert(
                                dependenciesAllowed,
                                "Mach DI error: user services cannot depend on Mach framework "
                                "types");

                            if constexpr (serviceAllowed && dependenciesAllowed) {
                                constexpr bool constructibleImplementation =
                                    std::is_constructible_v<T, Deps&...>;

                                static_assert(
                                    constructibleImplementation,
                                    "Mach DI error: service cannot be constructed from the "
                                    "declared dependencies");

                                if constexpr (constructibleImplementation) {

                                    if constexpr (serviceAllowed && dependenciesAllowed) {
                                        const std::type_index type = typeid(T);

                                        if (access != ServiceAccess::Internal && m_reservedTypes.contains(type)) {
                                            throw std::logic_error(
                                                std::format(
                                                    "Mach DI error: service '{}' is reserved for "
                                                    "internal use",
                                                    std::string(type.name())));
                                        }

                                        ServiceDescriptor descriptor{
                                            .type = type,
                                            .lifetime = lifetime,
                                            .access = access,
                                            .dependencies = {std::type_index(typeid(Deps))...},
                                            .factory = [](Scope& scope) {
                                                return std::make_shared<T>(
                                                    scope.resolve<Deps>()...);
                                            }};

                                        auto [_, inserted] =
                                            m_serviceRegistry.emplace(type, std::move(descriptor));

                                        if (!inserted) {
                                            throw std::logic_error(
                                                std::format(
                                                    "Mach DI error: duplicate service "
                                                    "registration: {}",
                                                    type.name()));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    template <typename T>
    T& Container::addSingletonInstance(T&& instance, ServiceAccess access) {
        const std::type_index type = typeid(T);

        if (m_serviceRegistry.contains(type) || m_singletonEntries.contains(type)) {
            throw std::logic_error("Mach error: service is already registered");
        }

        ServiceDescriptor descriptor{
            .type = type,
            .lifetime = ServiceLifetime::Singleton,
            .access = access};

        auto sharedInstance = std::make_shared<T>(std::move(instance));
        T& reference = *sharedInstance;

        auto [it, inserted] =
            m_singletonEntries.try_emplace(type, std::make_unique<SingletonEntry>());

        if (!inserted) {
            throw std::logic_error("Mach error: singleton entry already exists");
        }

        auto& entry = *it->second;

        std::call_once(
            entry.initializationFlag,
            [&entry, sharedInstance = std::move(sharedInstance)]() mutable {
                entry.instance = std::move(sharedInstance);
            });

        m_serviceRegistry.emplace(type, std::move(descriptor));

        return reference;
    }

    template <typename T>
    void Container::reserveInternal() {
        m_reservedTypes.insert(typeid(T));
    }
}
