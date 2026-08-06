#pragma once

#include <string_view>

#include <mach/detail/csrf/CsrfOptions.hpp>

namespace mach
{
    class AppBuilder;

    class CsrfBuilder {

    public:
        CsrfBuilder(const CsrfBuilder&) = delete;
        CsrfBuilder& operator=(const CsrfBuilder&) = delete;

        CsrfBuilder(CsrfBuilder&&) noexcept = default;
        CsrfBuilder& operator=(CsrfBuilder&&) noexcept = default;

        ~CsrfBuilder() = default;

        CsrfBuilder& cookieName(std::string_view name);

        CsrfBuilder& headerName(std::string_view name);

    private:
        CsrfBuilder() = default;

        detail::csrf::CsrfOptions&& takeOptions() &&;

        detail::csrf::CsrfOptions m_options;

        friend class AppBuilder;
    };
}
