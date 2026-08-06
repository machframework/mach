#include <mach/CsrfBuilder.hpp>

namespace mach
{
    CsrfBuilder& CsrfBuilder::cookieName(std::string_view name) {
        m_options.cookieName = name;
        return *this;
    }

    CsrfBuilder& CsrfBuilder::headerName(std::string_view name) {
        m_options.headerName = name;
        return *this;
    }

    detail::csrf::CsrfOptions&& CsrfBuilder::takeOptions() && {
        return std::move(m_options);
    }
}
