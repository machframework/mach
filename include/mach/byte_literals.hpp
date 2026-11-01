
#pragma once

#include <mach/detail/core/byte_literals.hpp>

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
