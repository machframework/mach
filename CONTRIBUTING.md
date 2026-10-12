# Contributing to Mach

Thank you for your interest in contributing to Mach!

Contributions of all kinds are welcome, including bug fixes, new features, performance improvements, documentation, and tests. This guide describes how to contribute and the coding standards used throughout the project.

Please review these guidelines before submitting a contribution. All pull requests are reviewed for correctness, maintainability, and alignment with Mach's architecture and design principles.

## Getting Started

1. Explore the repository, documentation, and [existing GitHub issues](https://github.com/machframework/mach/issues).
2. Review the [Mach roadmap](https://machframework.dev/roadmap/) to understand current priorities.
3. Find an issue that interests you, or propose a change using a new issue.
4. Discuss your intended contribution before starting work, as described below.
5. Fork the repository and create a dedicated branch for your changes.
6. Follow the coding conventions in this guide, then build and test your changes.
7. Open a pull request for review.

You do not need permission to fork Mach, experiment with its code, or develop independent modifications.

## Working on Issues

Before starting work, check whether a relevant issue already exists and communicate your intentions on GitHub.

### Issue Assignments

- **Unassigned issues** are generally available for contributions. Comment on the issue to discuss taking it on before beginning work.
- **Assigned issues** already have someone responsible for the work. Please coordinate before implementing the same task.
- **Issues labeled `help wanted`** are particularly suitable for external contributions.
- Issue assignments should be made once a contributor has agreed to take responsibility for the work.

If you'd like to help with an assigned issue, leave a comment or email the maintainer at [asaf@machframework.dev](mailto:asaf@machframework.dev).

### Sub-Issues

- Large features may be divided into sub-issues, with different contributors responsible for separate tasks.
- An assignment on a parent issue does not imply ownership of all its sub-issues.
- Check the assignment and discussion on the specific sub-issue before beginning work.

### Proposing Changes

Before beginning any contribution, including minor bug fixes and documentation improvements:

- Check whether an existing issue covers the proposed change.
- Comment on that issue, or open a new one, to explain what you'd like to do.
- For significant features, architectural changes, or modifications to public APIs, discuss the proposed implementation approach before starting development.

These discussions help coordinate development, avoid duplicated effort, and keep changes aligned with Mach's architecture and roadmap. Prior discussion or issue assignment does not guarantee that a contribution will be merged.

## Pull Requests

Contributions to the official Mach repository should be submitted through pull requests.

### Before Submitting

- Follow the coding conventions outlined in this document.
- Verify that the project builds successfully with your changes.
- Run relevant existing tests and add or update tests for changes in behavior.
- Document new or modified public APIs.
- Keep the pull request focused; exclude unrelated changes.
- Avoid unintended breaking changes to existing public APIs.

### Pull Request Description

Each pull request should include:

- A concise summary of the changes.
- A link to the relevant GitHub issue, if applicable.
- Important implementation decisions or trade-offs, when relevant.
- The tests performed and the platform(s) used.
- Any known limitations or remaining considerations.

### Code Review

- All pull requests require maintainer review before merging.
- Contributors may be asked to address correctness, maintainability, performance, consistency, or architectural concerns.
- Address review feedback with additional commits or discuss alternatives in the pull request.
- Final approval and merging are at the maintainer's discretion. A contribution may be declined if it does not meet project standards or align with the project's direction.

### Public API Compatibility

Mach follows semantic versioning.

- Preserve backward compatibility in existing public APIs unless a breaking change has been explicitly approved.
- Follow established naming, design, and documentation conventions for new public APIs.
- Discuss significant public API changes before implementation.

---

## General Principles

- Prefer readability and maintainability over premature optimization.
- Prefer explicit behavior over implicit behavior.
- Prefer simple solutions over unnecessary abstraction.

## Naming Conventions

- Class member fields should use the `m_name` convention.
- Local variables, function parameters, and public user-facing methods should use `camelCase`.
- Internal engine-level and infrastructure methods may use `snake_case`.
- Classes, structs, enums, and concepts should use `PascalCase`.
- Namespaces should use lowercase naming.

## Interfaces and Base Types

- Interface types should not use the `I` prefix.
- Base classes intended primarily for inheritance may use the `Base` suffix when appropriate.

## Project Structure

- Public headers belong under `include/mach/`.
- Private headers and implementation files should be placed under `src/` whenever possible.
- Internal headers that must be accessible to public headers belong under `include/mach/detail/`.
- Types and functions declared in these internal headers should use the `mach::detail` namespace or one of its nested namespaces.
- User-facing APIs should generally not expose implementation details.
- Code under `mach::detail` is considered internal and is not subject to public API compatibility guarantees.

## Header and Source Files

- Header files should use the `.hpp` extension.
- Implementation files should use the `.cpp` extension.
- Implementations should generally be placed in `.cpp` files instead of `.hpp` files.
- Header files should include only what they need.
- Prefer forward declarations in headers when possible.
- Prefer placing `#include` directives in `.cpp` files when the full type is only needed for implementation details.

## Header Guards

- Header files should use `#pragma once`.

## Include Order

Headers should be grouped in the following order:

1. Current translation unit header (if applicable)
2. Standard library headers
3. Third-party library headers
4. Project headers

Each group should be separated by a blank line, and headers within each group should be ordered lexicographically.

Example:

```cpp
#include "CurrentFile.hpp"

#include <chrono>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "mach/App.hpp"
#include "mach/Request.hpp"
```

## File Layout

- Public members should appear before private members.
- Constructors should appear before regular methods.
- Member fields should appear at the end of the class.

## Namespace Usage

- `using namespace` should not be used.
- Nested namespace declarations should use the modern namespace syntax as shown in the example.
- Fully qualified names should generally be preferred over specific `using` declarations.
- Specific `using` declarations may be used when they significantly improve readability or reduce verbosity.

Example:

```cpp
namespace project::module
{
    using std::string;
    using std::vector;

    // Avoid: using namespace std;
}
```

## Type Aliases

- `using` should be preferred over `typedef`.

---

## Braces

- Opening braces should use the same-line (K&R) style, except for namespace declarations.
- Namespace declarations should place the opening brace on a separate line.

Example:

```cpp
namespace mach::detail
{
    void initialize() {
        if (isReady()) {
            start();
        }
    }
}
```

## Control Statements

- Control statements should always use braces, even for single-line bodies.

Example:

```cpp
if (condition) {
    // ...
}

// Instead of:

if (condition)
    // ...
```

## Blank Lines

- Use blank lines to separate logical blocks.
- Prefer a blank line before a final `return` when it separates the result from the preceding logic.
- All files should end with a trailing newline.

## Pointer and Reference Style

- Pointer and reference symbols should be attached to the type.

Example:

```cpp
Type* pointer;
Type& reference;
```

---

## Templates

- `typename` should be preferred over `class` in template declarations.

## Const Usage

- Prefer `const` for values and references that should not be modified, particularly when it communicates an important invariant or constraint.
- Function parameters that are passed by reference and not modified should use `const Type&`.
- `const` should use the west-const style (`const Type`).

## Auto Usage

- `auto` should be used when the type is obvious from the right-hand side.
- `auto` should be used when the explicit type is unnecessarily verbose.
- Explicit types should be preferred when they improve readability or clarify intent.

Example:

```cpp
auto value = SomeType{};
auto iterator = values.begin();

int count = 10;
std::string text = "example";
```

## Numeric Types

- Prefer `std::size_t` for sizes, counts, and indexes.
- Prefer `int` for simple local counters when the range is small and non-negative guarantees are not important.
- Avoid `long` and `unsigned long` because their size differs between platforms.
- Prefer `std::int64_t` or `std::uint64_t` over `long long` when a 64-bit integer is required.
- Prefer `double` over `float` unless API compatibility requires `float`.
- Avoid unsigned integers unless the value is truly a size, bitmask, ID, or binary/protocol value.

## Data Types

- `struct` should be preferred for simple data-only types, internal DTOs, and aggregate-style objects.
- `class` should be preferred for types that enforce invariants, own behavior, hide implementation details, or expose a controlled public API.

## Enum Usage

- `enum class` should be preferred over plain enums.

## Override Usage

- Overridden virtual methods should explicitly use the `override` keyword.

Example:

```cpp
void execute() override;
```

## Explicit Constructors

- Single-argument constructors should use `explicit` unless implicit conversion is intentionally desired.

## Noexcept Usage

- `noexcept` should be used when a function is guaranteed not to throw.
- Move constructors and move assignment operators should use `noexcept` when they can guarantee not to throw.
- Destructors should not allow exceptions to escape.
- `noexcept` should not be added solely for theoretical performance benefits.
- Functions marked `noexcept` must not allow exceptions to escape, as doing so results in program termination.

## Function Parameters

- Functions should generally receive the smallest meaningful input they need.
- Prefer passing specific values over whole objects when only a small part of the object is required.
- Prefer passing the object when the function conceptually operates on that object or relies on its invariants.

## Performance

- Prefer passing large objects by `const Type&` instead of by value when ownership is not required.
- Prefer `std::string_view` for read-only string parameters when ownership is not needed and the referenced data remains valid.
- Prefer move semantics when transferring ownership or consuming values.
- Avoid unnecessary use of `std::move`, particularly on `const` objects or values that should remain unchanged.
- Avoid unnecessary heap allocation.
- Avoid unnecessary copies, especially in request/response paths.
- Prefer reserving container capacity when the expected size is known.
- Avoid blocking operations in asynchronous execution paths.
- Optimize only after correctness, clarity, and profiling.

## Ownership and Lifetime

- Prefer automatic lifetime management over manual lifetime management.
- Prefer values when ownership and lifetime permit.
- Prefer references or non-owning pointers when ownership is not being transferred.
- Prefer `std::unique_ptr` when a dynamically allocated object has a single owner.
- Prefer `std::shared_ptr` only when ownership must be shared across multiple entities or lifetimes.
- Raw pointers should generally express non-owning access rather than ownership.
- Avoid manual `new` and `delete` in application and framework code unless a specific low-level requirement exists.

## Special Member Functions

- Prefer the Rule of Zero whenever possible.
- Do not explicitly declare special member functions when the compiler-generated behavior is sufficient.
- Use `= default` when a special member function must be explicitly declared for clarity, visibility, or interface requirements.
- Use `= delete` when an operation should not be allowed.
- Implement the Rule of Five only when custom resource management is required.

---

## Thread Safety

- Thread ownership and synchronization responsibilities should be explicit.
- Shared mutable state should be minimized whenever possible.

## Documentation

- Public user-facing classes, structs, and methods should include documentation comments.
- Public classes and structs should document their purpose, responsibilities, ownership, and thread-safety characteristics when applicable.
- Public methods should document their behavior, parameters, return values, exceptions, and thread-safety characteristics when applicable.
- Internal/private classes and methods do not require the same formal documentation format, but non-obvious behavior, design decisions, ownership, lifetime, threading, and performance considerations should still be documented.
- Comments should explain intent and important constraints, not restate obvious code.
- Code should prioritize readability and self-documenting naming whenever possible.
- Comments should generally be placed above the relevant line or block instead of inline.
- Inline comments should only be used for short clarifications that improve readability.

Class documentation example:

```cpp
/**
 * Represents a built Mach application.
 *
 * Used to register routes and start the underlying HTTP server after
 * application configuration has been finalized by AppBuilder.
 *
 * Ownership:
 * - Owns the application's runtime state.
 * - Should be treated as the root runtime object of a Mach application.
 *
 * Thread safety:
 * - Not thread-safe.
 */
```

Method documentation example:

```cpp
/**
 * Registers a PUT request handler.
 *
 * @param pattern The route pattern to match (e.g. "/api/users").
 * @param handler The function invoked when the route is matched.
 *
 * @throws std::logic_error If a route with the same method and pattern
 *         has already been registered.
 *
 * @thread_safety This function is not thread-safe.
 */
```

## Commit Messages

- Commit messages should follow the Conventional Commits format: `type: description`.
- Descriptions should use the imperative mood (e.g., `add` instead of `added` or `adds`).
- Descriptions should begin with a lowercase letter.
- Descriptions should be concise and clearly explain the change.
- Commit message subjects should not end with a period.

Supported prefixes include:

- `feat` - New features.
- `fix` - Bug fixes.
- `docs` - Documentation changes.
- `perf` - Performance improvements.
- `refactor` - Code restructuring without changing behavior.
- `test` - Adding or updating tests.
- `bench` - Benchmark additions and improvements (Mach-specific convention).
- `build` - Build system and dependency changes.
- `chore` - Maintenance tasks that do not fit other categories.

Examples:

```text
feat: add configurable HTTP request limits
fix: prevent duplicate route registration
docs: update installation guide
refactor: simplify request dispatch logic
test: add request binding tests
bench: add HTTP throughput benchmarks
```
