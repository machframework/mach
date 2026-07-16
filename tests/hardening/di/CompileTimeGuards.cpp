#include <exception>
#include <string>
#include <string_view>

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>

#include "Testing.hpp"

#ifndef MACH_DI_COMPILE_FAILURE_TEST
#define MACH_DI_COMPILE_FAILURE_TEST 0
#endif

namespace
{
    using mach::detail::di::Container;
    using mach::detail::di::ServiceLifetime;

    // -------------------------------------------------------------------------
    // Valid service types
    // -------------------------------------------------------------------------

    class Dependency
    {};

    class SecondDependency
    {};

    class DefaultConstructibleService
    {};

    class ServiceWithDependency
    {
    public:
        explicit ServiceWithDependency(Dependency&)
        {}
    };

    class ServiceWithMultipleDependencies
    {
    public:
        ServiceWithMultipleDependencies(
            Dependency&,
            SecondDependency&
        ) {}
    };

    class FinalService final
    {};

    class NonCopyableService
    {
    public:
        NonCopyableService() = default;

        NonCopyableService(const NonCopyableService&) = delete;
        NonCopyableService& operator=(const NonCopyableService&) = delete;

        NonCopyableService(NonCopyableService&&) = delete;
        NonCopyableService& operator=(NonCopyableService&&) = delete;
    };

    class NonCopyableDependency
    {
    public:
        NonCopyableDependency() = default;

        NonCopyableDependency(const NonCopyableDependency&) = delete;
        NonCopyableDependency& operator=(const NonCopyableDependency&) = delete;
    };

    class ServiceWithNonCopyableDependency
    {
    public:
        explicit ServiceWithNonCopyableDependency(
            NonCopyableDependency&
        ) {}
    };

    // The actual construction expression T(Deps&...) permits this conversion.
    // This is therefore valid under Mach's current registration contract.
    class BaseDependency
    {};

    class DerivedDependency : public BaseDependency
    {};

    class ServiceAcceptingBaseDependency
    {
    public:
        explicit ServiceAcceptingBaseDependency(BaseDependency&)
        {}
    };

    // -------------------------------------------------------------------------
    // Invalid implementation types
    // -------------------------------------------------------------------------

    enum class EnumService
    {
        Value
    };

    union UnionService
    {
        int integer;
        double floatingPoint;
    };

    using FunctionService = void();

    class AbstractService
    {
    public:
        virtual ~AbstractService() = default;
        virtual void execute() = 0;
    };

    class DeletedDestructorService
    {
    public:
        DeletedDestructorService() = default;
        ~DeletedDestructorService() = delete;
    };

    class PrivateDestructorService
    {
    public:
        PrivateDestructorService() = default;

    private:
        ~PrivateDestructorService() = default;
    };

    class ProtectedDestructorService
    {
    public:
        ProtectedDestructorService() = default;

    protected:
        ~ProtectedDestructorService() = default;
    };

    class IncompleteService;

    // -------------------------------------------------------------------------
    // Invalid dependency types
    // -------------------------------------------------------------------------

    class AbstractDependency
    {
    public:
        virtual ~AbstractDependency() = default;
        virtual void execute() = 0;
    };

    class DeletedDestructorDependency
    {
    public:
        DeletedDestructorDependency() = default;
        ~DeletedDestructorDependency() = delete;
    };

    class PrivateDestructorDependency
    {
    public:
        PrivateDestructorDependency() = default;

    private:
        ~PrivateDestructorDependency() = default;
    };

    class ProtectedDestructorDependency
    {
    public:
        ProtectedDestructorDependency() = default;

    protected:
        ~ProtectedDestructorDependency() = default;
    };

    class IncompleteDependency;

    class GenericDependencyConsumer
    {
    public:
        template <typename T>
        explicit GenericDependencyConsumer(T&)
        {}
    };

    // -------------------------------------------------------------------------
    // Invalid constructor declarations
    // -------------------------------------------------------------------------

    class NoDefaultConstructor
    {
    public:
        explicit NoDefaultConstructor(int)
        {}
    };

    class WrongDependencyConstructor
    {
    public:
        explicit WrongDependencyConstructor(SecondDependency&)
        {}
    };

    class ConstructorTakesPointer
    {
    public:
        explicit ConstructorTakesPointer(Dependency*)
        {}
    };

    class ConstructorTakesRvalueReference
    {
    public:
        explicit ConstructorTakesRvalueReference(Dependency&&)
        {}
    };

    class DeletedDependencyConstructor
    {
    public:
        DeletedDependencyConstructor(Dependency&) = delete;
    };

    class PrivateDependencyConstructor
    {
    private:
        explicit PrivateDependencyConstructor(Dependency&)
        {}
    };

    class ProtectedDependencyConstructor
    {
    protected:
        explicit ProtectedDependencyConstructor(Dependency&)
        {}
    };

    // An lvalue Dependency cannot choose between these two constructors.
    class AmbiguousDependencyConstructor
    {
    public:
        explicit AmbiguousDependencyConstructor(const Dependency&)
        {}

        explicit AmbiguousDependencyConstructor(volatile Dependency&)
        {}
    };

    class TooFewDeclaredDependencies
    {
    public:
        TooFewDeclaredDependencies(
            Dependency&,
            SecondDependency&
        ) {}
    };

    class TooManyDeclaredDependencies
    {
    public:
        explicit TooManyDeclaredDependencies(Dependency&)
        {}
    };

    // -------------------------------------------------------------------------
    // Invalid dependency-pack relationships
    // -------------------------------------------------------------------------

    class DirectSelfDependency
    {
    public:
        explicit DirectSelfDependency(DirectSelfDependency&)
        {}
    };

    using DirectSelfDependencyAlias = DirectSelfDependency;

    class DuplicateDependencyConsumer
    {
    public:
        DuplicateDependencyConsumer(
            Dependency&,
            Dependency&
        ) {}
    };

    using DependencyAlias = Dependency;

    class DuplicateAliasedDependencyConsumer
    {
    public:
        DuplicateAliasedDependencyConsumer(
            Dependency&,
            DependencyAlias&
        ) {}
    };

    // -------------------------------------------------------------------------
    // Positive-test helpers
    // -------------------------------------------------------------------------

    template <typename T, typename... Deps>
    void expectRegistrationSuccess(
        std::string_view testName,
        ServiceLifetime lifetime
    ) {
        try {
            Container container;
            container.addService<T, Deps...>(lifetime);
            testing::success(testName);
        }
        catch (const std::exception& exception) {
            testing::fail(testName, exception.what());
        }
        catch (...) {
            testing::fail(
                testName,
                "Registration threw a non-standard exception"
            );
        }
    }

    void testDefaultConstructibleService()
    {
        expectRegistrationSuccess<DefaultConstructibleService>(
            "Default-constructible service",
            ServiceLifetime::Transient
        );
    }

    void testSingleDependency()
    {
        expectRegistrationSuccess<
            ServiceWithDependency,
            Dependency
        >(
            "Service with one dependency",
            ServiceLifetime::Scoped
        );
    }

    void testMultipleDependencies()
    {
        expectRegistrationSuccess<
            ServiceWithMultipleDependencies,
            Dependency,
            SecondDependency
        >(
            "Service with multiple dependencies",
            ServiceLifetime::Singleton
        );
    }

    void testFinalService()
    {
        expectRegistrationSuccess<FinalService>(
            "Final service",
            ServiceLifetime::Transient
        );
    }

    void testNonCopyableService()
    {
        expectRegistrationSuccess<NonCopyableService>(
            "Non-copyable and non-movable service",
            ServiceLifetime::Scoped
        );
    }

    void testNonCopyableDependency()
    {
        expectRegistrationSuccess<
            ServiceWithNonCopyableDependency,
            NonCopyableDependency
        >(
            "Non-copyable dependency injected by reference",
            ServiceLifetime::Scoped
        );
    }

    void testConstructibleThroughBaseReference()
    {
        expectRegistrationSuccess<
            ServiceAcceptingBaseDependency,
            DerivedDependency
        >(
            "Dependency convertible to constructor parameter",
            ServiceLifetime::Transient
        );
    }

    // -------------------------------------------------------------------------
    // Compile-failure selector
    //
    // Every selected branch must fail compilation.
    // -------------------------------------------------------------------------

    void instantiateExpectedCompileFailure()
    {
        Container container;

#if MACH_DI_COMPILE_FAILURE_TEST == 1

        // Non-class implementation: void.
        container.addService<void>(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 2

        // Non-class implementation: fundamental type.
        container.addService<int>(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 3

        // Non-class implementation: enum.
        container.addService<EnumService>(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 4

        // Non-class implementation: union.
        container.addService<UnionService>(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 5

        // Non-class implementation: function type.
        container.addService<FunctionService>(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 6

        // Pointer implementation.
        container.addService<DefaultConstructibleService*>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 7

        // Pointer-to-pointer implementation.
        container.addService<DefaultConstructibleService**>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 8

        // Lvalue-reference implementation.
        container.addService<DefaultConstructibleService&>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 9

        // Rvalue-reference implementation.
        container.addService<DefaultConstructibleService&&>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 10

        // Const implementation.
        container.addService<const DefaultConstructibleService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 11

        // Volatile implementation.
        container.addService<volatile DefaultConstructibleService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 12

        // Const-volatile implementation.
        container.addService<const volatile DefaultConstructibleService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 13

        // Explicitly forbidden value-like class.
        container.addService<std::string>(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 14

        // Explicitly forbidden view-like class.
        container.addService<std::string_view>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 15

        // Incomplete implementation.
        container.addService<IncompleteService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 16

        // Abstract implementation.
        container.addService<AbstractService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 17

        // Deleted implementation destructor.
        container.addService<DeletedDestructorService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 18

        // Private implementation destructor.
        container.addService<PrivateDestructorService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 19

        // Protected implementation destructor.
        container.addService<ProtectedDestructorService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 20

        // Non-class dependency: void.
        container.addService<GenericDependencyConsumer, void>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 21

        // Non-class dependency: fundamental type.
        container.addService<GenericDependencyConsumer, int>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 22

        // Non-class dependency: enum.
        container.addService<GenericDependencyConsumer, EnumService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 23

        // Non-class dependency: union.
        container.addService<GenericDependencyConsumer, UnionService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 24

        // Non-class dependency: function type.
        container.addService<GenericDependencyConsumer, FunctionService>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 25

        // Pointer dependency.
        container.addService<
            GenericDependencyConsumer,
            Dependency*
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 26

        // Pointer-to-pointer dependency.
        container.addService<
            GenericDependencyConsumer,
            Dependency**
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 27

        // Lvalue-reference dependency declaration.
        container.addService<
            GenericDependencyConsumer,
            Dependency&
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 28

        // Rvalue-reference dependency declaration.
        container.addService<
            GenericDependencyConsumer,
            Dependency&&
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 29

        // Const dependency.
        container.addService<
            GenericDependencyConsumer,
            const Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 30

        // Volatile dependency.
        container.addService<
            GenericDependencyConsumer,
            volatile Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 31

        // Const-volatile dependency.
        container.addService<
            GenericDependencyConsumer,
            const volatile Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 32

        // Explicitly forbidden dependency type.
        container.addService<
            GenericDependencyConsumer,
            std::string
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 33

        // Explicitly forbidden dependency type.
        container.addService<
            GenericDependencyConsumer,
            std::string_view
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 34

        // Incomplete dependency.
        container.addService<
            GenericDependencyConsumer,
            IncompleteDependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 35

        // Abstract dependency.
        container.addService<
            GenericDependencyConsumer,
            AbstractDependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 36

        // Dependency with deleted destructor.
        container.addService<
            GenericDependencyConsumer,
            DeletedDestructorDependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 37

        // Dependency with private destructor.
        container.addService<
            GenericDependencyConsumer,
            PrivateDestructorDependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 38

        // Dependency with protected destructor.
        container.addService<
            GenericDependencyConsumer,
            ProtectedDestructorDependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 39

        // Empty dependency pack, but no default constructor.
        container.addService<NoDefaultConstructor>(
            ServiceLifetime::Transient
        );

#elif MACH_DI_COMPILE_FAILURE_TEST == 40

        // Declared dependency does not match the available constructor.
        container.addService<
            WrongDependencyConstructor,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 41

        // Mach supplies Dependency&, but the constructor requires Dependency*.
        container.addService<
            ConstructorTakesPointer,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 42

        // Mach supplies an lvalue, but the constructor requires Dependency&&.
        container.addService<
            ConstructorTakesRvalueReference,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 43

        // Selected constructor is deleted.
        container.addService<
            DeletedDependencyConstructor,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 44

        // Selected constructor is private.
        container.addService<
            PrivateDependencyConstructor,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 45

        // Selected constructor is protected.
        container.addService<
            ProtectedDependencyConstructor,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 46

        // Construction from Dependency& is ambiguous.
        container.addService<
            AmbiguousDependencyConstructor,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 47

        // Constructor requires two dependencies, but only one was declared.
        container.addService<
            TooFewDeclaredDependencies,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 48

        // Constructor accepts one dependency, but two were declared.
        container.addService<
            TooManyDeclaredDependencies,
            Dependency,
            SecondDependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 49

        // Direct self-dependency.
        container.addService<
            DirectSelfDependency,
            DirectSelfDependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 50

        // Direct self-dependency hidden behind a type alias.
        container.addService<
            DirectSelfDependency,
            DirectSelfDependencyAlias
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 51

        // Duplicate dependency types.
        container.addService<
            DuplicateDependencyConsumer,
            Dependency,
            Dependency
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST == 52

        // Duplicate dependency hidden behind a type alias.
        container.addService<
            DuplicateAliasedDependencyConsumer,
            Dependency,
            DependencyAlias
        >(ServiceLifetime::Transient);

#elif MACH_DI_COMPILE_FAILURE_TEST != 0

#error Unknown MACH_DI_COMPILE_FAILURE_TEST value

#endif
    }
}

int main()
{
#if MACH_DI_COMPILE_FAILURE_TEST == 0
    testDefaultConstructibleService();
    testSingleDependency();
    testMultipleDependencies();
    testFinalService();
    testNonCopyableService();
    testNonCopyableDependency();
    testConstructibleThroughBaseReference();
#else
    // This call exists only to instantiate the selected invalid registration.
    // A successful compilation means the firewall was bypassed.
    instantiateExpectedCompileFailure();

    testing::fail(
        "DI compile-time firewall",
        "An invalid registration unexpectedly compiled"
    );
#endif

    return 0;
}