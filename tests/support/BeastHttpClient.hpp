#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace testing::http
{
    class BeastHttpClient {

    public:
        class Response {

        public:
            [[nodiscard]] int statusCode() const noexcept;
            [[nodiscard]] const std::string& body() const noexcept;

        private:
            friend class BeastHttpClient;

            Response(int statusCode, std::string body);

            int statusCode_;
            std::string body_;
        };

        BeastHttpClient(std::string host = "127.0.0.1", std::uint16_t port = 3143);

        ~BeastHttpClient();

        BeastHttpClient(BeastHttpClient&&) noexcept;
        BeastHttpClient& operator=(BeastHttpClient&&) noexcept;

        BeastHttpClient(const BeastHttpClient&) = delete;
        BeastHttpClient& operator=(const BeastHttpClient&) = delete;

        [[nodiscard]] Response get(std::string_view target);

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };
}
