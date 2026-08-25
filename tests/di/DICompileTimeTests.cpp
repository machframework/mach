#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

#include <string>
#include <string_view>

#ifndef MACH_DI_COMPILE_FAILURE_TEST
#define MACH_DI_COMPILE_FAILURE_TEST 0
#endif

namespace
{
    namespace di = mach::detail::di;

    // -------------------------------------------------------------------------
    // Valid registrations
    // -------------------------------------------------------------------------

    struct Dependency {};

    struct SecondDependency {};

    struct DefaultConstructibleService {};

    struct ServiceWithDependency {
        explicit ServiceWithDependency(Dependency&) {}
    };

    struct ServiceWithMultipleDependencies {
        ServiceWithMultipleDependencies(Dependency&, SecondDependency&) {}
    };

    struct FinalService final {};

    struct NonCopyableService {
        NonCopyableService() = default;

        NonCopyableService(const NonCopyableService&) = delete;
        NonCopyableService& operator=(const NonCopyableService&) = delete;

        NonCopyableService(NonCopyableService&&) = delete;
        NonCopyableService& operator=(NonCopyableService&&) = delete;
    };

    struct NonCopyableDependency {
        NonCopyableDependency() = default;

        NonCopyableDependency(const NonCopyableDependency&) = delete;
        NonCopyableDependency& operator=(const NonCopyableDependency&) = delete;

        NonCopyableDependency(NonCopyableDependency&&) = delete;
        NonCopyableDependency& operator=(NonCopyableDependency&&) = delete;
    };

    struct ServiceWithNonCopyableDependency {
        explicit ServiceWithNonCopyableDependency(NonCopyableDependency&) {}
    };

    struct BaseDependency {};

    struct DerivedDependency : BaseDependency {};

    struct ServiceAcceptingBaseDependency {
        explicit ServiceAcceptingBaseDependency(BaseDependency&) {}
    };

    // -------------------------------------------------------------------------
    // Invalid implementation types
    // -------------------------------------------------------------------------

    enum class EnumService {
        Value
    };

    union UnionService {
        int integer;
        double floatingPoint;
    };

    using FunctionService = void();

    struct AbstractService {
        virtual ~AbstractService() = default;
        virtual void execute() = 0;
    };

    struct DeletedDestructorService {
        ~DeletedDestructorService() = delete;
    };

    struct PrivateDestructorService {
    private:
        ~PrivateDestructorService() = default;
    };

    struct ProtectedDestructorService {
    protected:
        ~ProtectedDestructorService() = default;
    };

    struct IncompleteService;

    // -------------------------------------------------------------------------
    // Invalid dependency types
    // -------------------------------------------------------------------------

    struct AbstractDependency {
        virtual ~AbstractDependency() = default;
        virtual void execute() = 0;
    };

    struct DeletedDestructorDependency {
        ~DeletedDestructorDependency() = delete;
    };

    struct PrivateDestructorDependency {
    private:
        ~PrivateDestructorDependency() = default;
    };

    struct ProtectedDestructorDependency {
    protected:
        ~ProtectedDestructorDependency() = default;
    };

    struct IncompleteDependency;

    struct GenericDependencyConsumer {
        template <typename T>
        explicit GenericDependencyConsumer(T&) {}
    };

    // -------------------------------------------------------------------------
    // Invalid constructors
    // -------------------------------------------------------------------------

    struct NoDefaultConstructor {
        explicit NoDefaultConstructor(int) {}
    };

    struct WrongDependencyConstructor {
        explicit WrongDependencyConstructor(SecondDependency&) {}
    };

    struct ConstructorTakesPointer {
        explicit ConstructorTakesPointer(Dependency*) {}
    };

    struct ConstructorTakesRvalueReference {
        explicit ConstructorTakesRvalueReference(Dependency&&) {}
    };

    struct DeletedDependencyConstructor {
        DeletedDependencyConstructor(Dependency&) = delete;
    };

    struct PrivateDependencyConstructor {
    private:
        explicit PrivateDependencyConstructor(Dependency&) {}
    };

    struct ProtectedDependencyConstructor {
    protected:
        explicit ProtectedDependencyConstructor(Dependency&) {}
    };

    struct AmbiguousDependencyConstructor {
        explicit AmbiguousDependencyConstructor(const Dependency&) {}

        explicit AmbiguousDependencyConstructor(volatile Dependency&) {}
    };

    struct TooFewDeclaredDependencies {
        TooFewDeclaredDependencies(Dependency&, SecondDependency&) {}
    };

    struct TooManyDeclaredDependencies {
        explicit TooManyDeclaredDependencies(Dependency&) {}
    };

    // -------------------------------------------------------------------------
    // Invalid dependency packs
    // -------------------------------------------------------------------------

    struct DirectSelfDependency {
        explicit DirectSelfDependency(DirectSelfDependency&) {}
    };

    using DirectSelfDependencyAlias = DirectSelfDependency;

    struct DuplicateDependencyConsumer {
        DuplicateDependencyConsumer(Dependency&, Dependency&) {}
    };

    using DependencyAlias = Dependency;

    struct DuplicateAliasedDependencyConsumer {
        DuplicateAliasedDependencyConsumer(Dependency&, DependencyAlias&) {}
    };

    // -------------------------------------------------------------------------
    // Positive compilation coverage
    // -------------------------------------------------------------------------

    void instantiateValidRegistrations() {
        {
            di::Container container;

            container.addService<DefaultConstructibleService>(di::ServiceLifetime::Transient);
        }

        {
            di::Container container;

            container.addService<ServiceWithDependency, Dependency>(di::ServiceLifetime::Scoped);
        }

        {
            di::Container container;

            container.addService<ServiceWithMultipleDependencies, Dependency, SecondDependency>(
                di::ServiceLifetime::Singleton);
        }

        {
            di::Container container;

            container.addService<FinalService>(di::ServiceLifetime::Transient);
        }

        {
            di::Container container;

            container.addService<NonCopyableService>(di::ServiceLifetime::Scoped);
        }

        {
            di::Container container;

            container.addService<ServiceWithNonCopyableDependency, NonCopyableDependency>(
                di::ServiceLifetime::Scoped);
        }

        {
            di::Container container;

            container.addService<ServiceAcceptingBaseDependency, DerivedDependency>(
                di::ServiceLifetime::Transient);
        }
    }

    // -------------------------------------------------------------------------
    // Compile-failure selector
    //
    // Every non-zero branch must fail compilation.
    // -------------------------------------------------------------------------

    void instantiateExpectedCompileFailure() {
        di::Container container;

#if MACH_DI_COMPILE_FAILURE_TEST == 1

        container.addService<void>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 2

        container.addService<int>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 3

        container.addService<EnumService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 4

        container.addService<UnionService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 5

        container.addService<FunctionService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 6

        container.addService<DefaultConstructibleService*>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 7

        container.addService<DefaultConstructibleService**>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 8

        container.addService<DefaultConstructibleService&>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 9

        container.addService<DefaultConstructibleService&&>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 10

        container.addService<const DefaultConstructibleService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 11

        container.addService<volatile DefaultConstructibleService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 12

        container.addService<const volatile DefaultConstructibleService>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 13

        container.addService<std::string>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 14

        container.addService<std::string_view>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 15

        container.addService<IncompleteService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 16

        container.addService<AbstractService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 17

        container.addService<DeletedDestructorService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 18

        container.addService<PrivateDestructorService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 19

        container.addService<ProtectedDestructorService>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 20

        container.addService<GenericDependencyConsumer, void>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 21

        container.addService<GenericDependencyConsumer, int>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 22

        container.addService<GenericDependencyConsumer, EnumService>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 23

        container.addService<GenericDependencyConsumer, UnionService>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 24

        container.addService<GenericDependencyConsumer, FunctionService>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 25

        container.addService<GenericDependencyConsumer, Dependency*>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 26

        container.addService<GenericDependencyConsumer, Dependency**>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 27

        container.addService<GenericDependencyConsumer, Dependency&>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 28

        container.addService<GenericDependencyConsumer, Dependency&&>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 29

        container.addService<GenericDependencyConsumer, const Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 30

        container.addService<GenericDependencyConsumer, volatile Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 31

        container.addService<GenericDependencyConsumer, const volatile Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 32

        container.addService<GenericDependencyConsumer, std::string>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 33

        container.addService<GenericDependencyConsumer, std::string_view>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 34

        container.addService<GenericDependencyConsumer, IncompleteDependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 35

        container.addService<GenericDependencyConsumer, AbstractDependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 36

        container.addService<GenericDependencyConsumer, DeletedDestructorDependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 37

        container.addService<GenericDependencyConsumer, PrivateDestructorDependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 38

        container.addService<GenericDependencyConsumer, ProtectedDestructorDependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 39

        container.addService<NoDefaultConstructor>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 40

        container.addService<WrongDependencyConstructor, Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 41

        container.addService<ConstructorTakesPointer, Dependency>(di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 42

        container.addService<ConstructorTakesRvalueReference, Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 43

        container.addService<DeletedDependencyConstructor, Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 44

        container.addService<PrivateDependencyConstructor, Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 45

        container.addService<ProtectedDependencyConstructor, Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 46

        container.addService<AmbiguousDependencyConstructor, Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 47

        container.addService<TooFewDeclaredDependencies, Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 48

        container.addService<TooManyDeclaredDependencies, Dependency, SecondDependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 49

        container.addService<DirectSelfDependency, DirectSelfDependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 50

        container.addService<DirectSelfDependency, DirectSelfDependencyAlias>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 51

        container.addService<DuplicateDependencyConsumer, Dependency, Dependency>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 52

        container.addService<DuplicateAliasedDependencyConsumer, Dependency, DependencyAlias>(
            di::ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST != 0

#error Unknown MACH_DI_COMPILE_FAILURE_TEST value

#endif
    }
}

int main() {
#if MACH_DI_COMPILE_FAILURE_TEST == 0
    instantiateValidRegistrations();
#else
    instantiateExpectedCompileFailure();
#endif

    return 0;
}
