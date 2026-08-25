#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace testing::http
{
    class BeastHttpClient {

    public:
        using Headers = std::unordered_map<std::string, std::string>;

        class Response {

        public:
            [[nodiscard]] int statusCode() const noexcept;
            [[nodiscard]] const std::string& body() const noexcept;

        private:
            friend class BeastHttpClient;

            Response(int statusCode, std::string body);

            int m_statusCode;
            std::string m_body;
        };

        BeastHttpClient(std::string host = "127.0.0.1", std::uint16_t port = 3143);

        ~BeastHttpClient();

        BeastHttpClient(BeastHttpClient&&) noexcept;
        BeastHttpClient& operator=(BeastHttpClient&&) noexcept;

        BeastHttpClient(const BeastHttpClient&) = delete;
        BeastHttpClient& operator=(const BeastHttpClient&) = delete;

        [[nodiscard]]
        Response get(std::string_view target, const Headers& headers = {});

        [[nodiscard]]
        Response post(
            std::string_view target,
            std::string_view body = "",
            const Headers& headers = {});

        [[nodiscard]]
        Response put(
            std::string_view target,
            std::string_view body = "",
            const Headers& headers = {});

        [[nodiscard]]
        Response patch(
            std::string_view target,
            std::string_view body = "",
            const Headers& headers = {});

        [[nodiscard]]
        Response del(std::string_view target, const Headers& headers = {});

        [[nodiscard]]
        Response head(std::string_view target, const Headers& headers = {});

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
