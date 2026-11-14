#include "TestRequestBuilder.hpp"

#include <stdexcept>
#include <utility>

#include <boost/beast/http.hpp>

#include "../../src/adapter/inbound/BeastRequestAdapter.hpp"

namespace testing::http
{
    namespace beast = boost::beast;
    namespace http = beast::http;

    namespace
    {
        http::verb toBeastVerb(mach::http::Method method) {
            using Method = mach::http::Method;

            switch (method) {
            case Method::Get:
                return http::verb::get;
            case Method::Post:
                return http::verb::post;
            case Method::Put:
                return http::verb::put;
            case Method::Patch:
                return http::verb::patch;
            case Method::Delete:
                return http::verb::delete_;
            case Method::Head:
                return http::verb::head;
            case Method::Options:
                return http::verb::options;
            default:
                throw std::invalid_argument("TestRequestBuilder: unsupported HTTP method");
            }
        }

        beast::string_view toBeastView(std::string_view value) {
            return {value.data(), value.size()};
        }
    }

    class TestRequestBuilder::Impl {
    public:
        http::request<http::string_body> request{http::verb::get, "/", 11};
    };

    TestRequestBuilder::TestRequestBuilder() : m_impl(std::make_unique<Impl>()) {}

    TestRequestBuilder::~TestRequestBuilder() = default;

    TestRequestBuilder::TestRequestBuilder(TestRequestBuilder&&) noexcept = default;

    TestRequestBuilder& TestRequestBuilder::operator=(TestRequestBuilder&&) noexcept = default;

    TestRequestBuilder&& TestRequestBuilder::method(mach::http::Method method) && {
        m_impl->request.method(toBeastVerb(method));
        return std::move(*this);
    }

    TestRequestBuilder&& TestRequestBuilder::target(std::string_view target) && {
        m_impl->request.target(toBeastView(target));
        return std::move(*this);
    }

    TestRequestBuilder&& TestRequestBuilder::body(std::string body) && {
        m_impl->request.body() = std::move(body);
        return std::move(*this);
    }

    TestRequestBuilder&& TestRequestBuilder::header(
        std::string_view name,
        std::string_view value) && {
        m_impl->request.set(toBeastView(name), toBeastView(value));

        return std::move(*this);
    }

    mach::Context TestRequestBuilder::build() && {
        m_impl->request.prepare_payload();

        mach::detail::http::adapter::BeastRequestAdapter adapter;
        bool rejected = false;

        return adapter.adapt(std::move(m_impl->request), rejected);
    }
}
