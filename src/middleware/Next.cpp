#include <mach/middleware/Next.hpp>

namespace mach
{
    Next::Next(std::function<void()> next) : m_next(std::move(next)) {}

    void Next::operator()() {
        if (m_invoked) {
            throw std::logic_error(
                "Mach error: next() may only be invoked once per middleware execution.");
        }

        m_invoked = true;
        m_next();
    }
}
