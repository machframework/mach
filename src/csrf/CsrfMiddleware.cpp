#include "CsrfMiddleware.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>

#ifdef _WIN32
    #define NOMINMAX
    #include <Windows.h>
    #include <bcrypt.h>
#elif defined(__linux__)
    #include <sys/random.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    #include <stdlib.h>
#else
    #error Unsupported platform
#endif

#include <mach/Context.hpp>
#include <mach/http/Cookie.hpp>
#include <mach/middleware/Next.hpp>

namespace mach::detail::csrf
{
    CsrfMiddleware::CsrfMiddleware(CsrfOptions& options) : m_options(options) {}

    void CsrfMiddleware::invoke(mach::Context& context, mach::Next& next) {
        const auto tokenCookie = context.request.cookie(m_options.cookieName);

        const auto method = context.request.method();

        // safe method
        if (method == mach::http::Method::Get || method == mach::http::Method::Head ||
            method == mach::http::Method::Options) {
            if (!tokenCookie) {
                auto cookie = mach::http::Cookie{
                    .name = m_options.cookieName,
                    .value = generateToken(),
                    .secure = true,

                };

                context.response.addCookie(std::move(cookie));
            }

            next();
            return;
        }

        // unsafe method
        const auto tokenHeader = context.request.header(m_options.headerName);

        if (!tokenHeader || !tokenCookie || *tokenHeader != *tokenCookie) {
            context.response.status(mach::http::StatusCode::Forbidden);
            context.response.body("CSRF token validation failed");
            return;
        }

        if (*tokenHeader != *tokenCookie) {
            context.response.status(mach::http::StatusCode::Forbidden);
            context.response.body("CSRF token validation failed");
            return;
        }
    }

    std::string CsrfMiddleware::generateToken() const {
        constexpr std::size_t tokenSize = 32;
        constexpr char hexDigits[] = "0123456789abcdef";

        std::array<std::uint8_t, tokenSize> bytes{};

#ifdef _WIN32

        const NTSTATUS status = BCryptGenRandom(
            nullptr,
            bytes.data(),
            static_cast<ULONG>(bytes.size()),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG);

        if (!BCRYPT_SUCCESS(status)) {
            throw std::runtime_error("Failed to generate a cryptographically secure random token.");
        }

#elif defined(__linux__)

        std::size_t offset = 0;

        while (offset < bytes.size()) {
            const ssize_t result = getrandom(bytes.data() + offset, bytes.size() - offset, 0);

            if (result < 0) {
                if (errno == EINTR) {
                    continue;
                }

                throw std::runtime_error(
                    "Failed to generate a cryptographically secure random token.");
            }

            offset += static_cast<std::size_t>(result);
        }

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

        arc4random_buf(bytes.data(), bytes.size());

#endif

        std::string token;
        token.reserve(tokenSize * 2);

        for (const auto byte : bytes) {
            token.push_back(hexDigits[byte >> 4]);
            token.push_back(hexDigits[byte & 0x0F]);
        }

        return token;
    }
}
