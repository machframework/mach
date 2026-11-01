
#pragma once

#include <mach/detail/core/byte_literals.hpp>

/**
 * Provides compile-time byte-size literals using SI and IEC units.
 *
 * All literals return std::size_t and reject values exceeding its
 * maximum representable value at compile time.
 *
 * Use `using namespace mach::literals;` to access the literals directly.
 */
namespace mach::literals
{
    consteval std::size_t operator"" _B(unsigned long long value) {
        return detail::checkedByteCount(value, 1);
    }

    // SI units (base 1000)

    consteval std::size_t operator"" _kB(unsigned long long value) {
        return detail::checkedByteCount(value, 1000);
    }

    consteval std::size_t operator"" _MB(unsigned long long value) {
        return detail::checkedByteCount(value, 1000ULL * 1000);
    }

    consteval std::size_t operator"" _GB(unsigned long long value) {
        return detail::checkedByteCount(value, 1000ULL * 1000 * 1000);
    }

    consteval std::size_t operator"" _TB(unsigned long long value) {
        return detail::checkedByteCount(value, 1000ULL * 1000 * 1000 * 1000);
    }

    // IEC units (base 1024)

    consteval std::size_t operator"" _KiB(unsigned long long value) {
        return detail::checkedByteCount(value, 1024);
    }

    consteval std::size_t operator"" _MiB(unsigned long long value) {
        return detail::checkedByteCount(value, 1024ULL * 1024);
    }

    consteval std::size_t operator"" _GiB(unsigned long long value) {
        return detail::checkedByteCount(value, 1024ULL * 1024 * 1024);
    }

    consteval std::size_t operator"" _TiB(unsigned long long value) {
        return detail::checkedByteCount(value, 1024ULL * 1024 * 1024 * 1024);
    }
}
