
#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <mach/Context.hpp>
#include <mach/http/Method.hpp>

namespace testing::http
{
    class TestRequestBuilder {
    public:
        TestRequestBuilder();

        ~TestRequestBuilder();

        TestRequestBuilder(TestRequestBuilder&&) noexcept;
        TestRequestBuilder& operator=(TestRequestBuilder&&) noexcept;

        TestRequestBuilder(const TestRequestBuilder&) = delete;
        TestRequestBuilder& operator=(const TestRequestBuilder&) = delete;

        TestRequestBuilder&& method(mach::http::Method method) &&;
        TestRequestBuilder&& target(std::string_view target) &&;
        TestRequestBuilder&& body(std::string body) &&;

        TestRequestBuilder&& header(std::string_view name, std::string_view value) &&;

        [[nodiscard]] mach::Context build() &&;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
