## General Principles

- Prefer readability and maintainability over premature optimization.
- Prefer explicit behavior over implicit behavior.
- Prefer simple solutions over unnecessary abstraction.

---

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
- Internal implementation belongs under `src/`.
- User-facing APIs should generally not expose implementation details.

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

- Headers should be grouped in the following order:
  1. Current translation unit header
  2. Standard library headers
  3. Third-party library headers
  4. Project headers
  5. Lexical order

Each group should be separated by a blank line.

Example:

```cpp
#include "CurrentFile.hpp"

#include <string>

#include <thirdparty/library.hpp>

#include "project/module/Header.hpp"
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
namespace project::module {
}

using std::string;
using std::vector;

//instead of:

using namespace std;
```

## Type Aliases

- `using` should be preferred over `typedef`.

---

## Braces

- Opening braces should use the same-line (K&R) style.

Example:

```cpp
class Router {
public:
    void addRoute() {
        if (condition) {
            // ...
        }
    }
};
```

## Control Statements

- Control statements should always use braces, even for single-line bodies.

Example:

```cpp
if (condition) {
    //...
}

//instead of:

if (condition)
    //...
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

- `const` should be used whenever a value is not intended to be modified.
- Function parameters that are passed by reference and not modified should use `const Type&`.
- `const` should use the west-const style (`const Type`).

## Auto Usage

- auto should be used when the type is obvious from the right-hand side.
- auto should be used when the explicit type is unnecessarily verbose.
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
- Destructors and move operations should use `noexcept` when possible.
- `noexcept` should not be added only for theoretical performance benefits.

## Function Parameters

- Functions should generally receive the smallest meaningful input they need.
- Prefer passing specific values over whole objects when only a small part of the object is required.
- Prefer passing the object when the function conceptually operates on that object or relies on its invariants.

## Performance

- Prefer passing large objects by `const Type&` instead of by value.
- Prefer `std::string_view` for read-only string parameters when ownership is not needed.
- Prefer move semantics when transferring ownership or consuming values.
- Avoid unnecessary heap allocation.
- Avoid unnecessary copies, especially in request/response paths.
- Prefer reserving container capacity when the expected size is known.
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

- Public user-facing APIs should include documentation comments as shown in the example below.
- Internal/private functions should only be documented when their behavior, reasoning, ownership, lifetime, threading, or performance details are non-obvious.
- Comments should explain intent and important constraints, not restate obvious code.
- Code should prioritize readability and self-documenting naming whenever possible.
- Comments should generally be placed above the relevant line or block instead of inline.
- Inline comments should only be used for short clarifications that improve readability.

Example:

```cpp
/**
 * Short summary of what the function does.
 *
 * More detailed explanation if needed (why, not how).
 *
 * @param parameter Description of the parameter.
 *
 * @return Description of the return value.
 *
 * @throws Description of any exceptions thrown (if applicable).
 *
 * @thread_safety Whether this function is thread-safe or requires external synchronization.
 *
 * @ownership Notes about ownership / lifetime transfer (if applicable).
 *
 * @notes Any important constraints, invariants, or side effects.
 */
```

---

## Commit Messages

- Commit messages should follow the conventional commit style.

Supported prefixes include:

- `feat`
- `fix`
- `docs`
- `perf`
- `refactor`
- `test`
- `build`
- `chore`
