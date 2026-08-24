#include "BeastHttpClient.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <utility>

namespace testing::http
{
    namespace asio = boost::asio;
    namespace beast = boost::beast;
    namespace http = beast::http;
    using tcp = asio::ip::tcp;

    class BeastHttpClient::Impl {

    public:
        Impl(std::string host, std::uint16_t port);

        Response get(std::string_view target);
        Response post(std::string_view target, std::string_view body);
        Response put(std::string_view target, std::string_view body);
        Response patch(std::string_view target, std::string_view body);
        Response del(std::string_view target);
        Response head(std::string_view target);

    private:
        void ensureConnected();
        std::string m_host;
        std::string port_;

        asio::io_context ioContext_;
        tcp::resolver resolver_;
        beast::tcp_stream m_stream;
    };

    BeastHttpClient::Impl::Impl(std::string host, std::uint16_t port)
        : m_host(std::move(host)), port_(std::to_string(port)), resolver_(ioContext_),
          m_stream(ioContext_) {}

    BeastHttpClient::Response BeastHttpClient::Impl::get(std::string_view target) {
        ensureConnected();

        http::request<http::empty_body> request{http::verb::get, target, 11};

        request.set(http::field::host, m_host);
        request.set(http::field::user_agent, "Mach Test Client");

        http::write(m_stream, request);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;

        http::read(m_stream, buffer, response);

        return Response{static_cast<int>(response.result_int()), std::move(response.body())};
    }

    BeastHttpClient::Response BeastHttpClient::Impl::post(
        std::string_view target,
        std::string_view body) {

        ensureConnected();

        http::request<http::string_body> request{http::verb::post, target, 11};

        request.set(http::field::host, m_host);
        request.set(http::field::user_agent, "Mach Test Client");
        request.set(http::field::content_type, "application/json");

        request.body() = body;
        request.prepare_payload();

        http::write(m_stream, request);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;

        http::read(m_stream, buffer, response);

        return Response{static_cast<int>(response.result_int()), std::move(response.body())};
    }

    BeastHttpClient::Response BeastHttpClient::Impl::put(
        std::string_view target,
        std::string_view body) {

        ensureConnected();

        http::request<http::string_body> request{http::verb::put, target, 11};

        request.set(http::field::host, m_host);
        request.set(http::field::user_agent, "Mach Test Client");
        request.set(http::field::content_type, "application/json");

        request.body() = body;
        request.prepare_payload();

        http::write(m_stream, request);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;

        http::read(m_stream, buffer, response);

        return Response{static_cast<int>(response.result_int()), std::move(response.body())};
    }

    BeastHttpClient::Response BeastHttpClient::Impl::patch(
        std::string_view target,
        std::string_view body) {

        ensureConnected();

        http::request<http::string_body> request{http::verb::patch, target, 11};

        request.set(http::field::host, m_host);
        request.set(http::field::user_agent, "Mach Test Client");
        request.set(http::field::content_type, "application/json");

        request.body() = body;
        request.prepare_payload();

        http::write(m_stream, request);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;

        http::read(m_stream, buffer, response);

        return Response{static_cast<int>(response.result_int()), std::move(response.body())};
    }

    BeastHttpClient::Response BeastHttpClient::Impl::del(std::string_view target) {
        ensureConnected();

        http::request<http::empty_body> request{http::verb::delete_, target, 11};

        request.set(http::field::host, m_host);
        request.set(http::field::user_agent, "Mach Test Client");

        http::write(m_stream, request);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;

        http::read(m_stream, buffer, response);

        return Response{static_cast<int>(response.result_int()), std::move(response.body())};
    }

    BeastHttpClient::Response BeastHttpClient::Impl::head(std::string_view target) {
        ensureConnected();

        http::request<http::empty_body> request{http::verb::head, target, 11};

        request.set(http::field::host, m_host);
        request.set(http::field::user_agent, "Mach Test Client");

        http::write(m_stream, request);

        beast::flat_buffer buffer;
        http::response_parser<http::string_body> parser;
        parser.skip(true);

        http::read(m_stream, buffer, parser);

        auto response = parser.release();

        return Response{static_cast<int>(response.result_int()), std::move(response.body())};
    }

    void BeastHttpClient::Impl::ensureConnected() {
        if (m_stream.socket().is_open()) {
            return;
        }

        const auto endpoints = resolver_.resolve(m_host, port_);
        m_stream.connect(endpoints);
    }

    BeastHttpClient::Response::Response(int statusCode, std::string body)
        : m_statusCode(statusCode), m_body(std::move(body)) {}

    int BeastHttpClient::Response::statusCode() const noexcept {
        return m_statusCode;
    }

    const std::string& BeastHttpClient::Response::body() const noexcept {
        return m_body;
    }

    BeastHttpClient::BeastHttpClient(std::string host, std::uint16_t port)
        : m_impl(std::make_unique<Impl>(std::move(host), port)) {}

    BeastHttpClient::~BeastHttpClient() = default;

    BeastHttpClient::BeastHttpClient(BeastHttpClient&&) noexcept = default;

    BeastHttpClient& BeastHttpClient::operator=(BeastHttpClient&&) noexcept = default;

    BeastHttpClient::Response BeastHttpClient::get(std::string_view target) {
        return m_impl->get(target);
    }

    BeastHttpClient::Response BeastHttpClient::post(
        std::string_view target,
        std::string_view body) {

        return m_impl->post(target, body);
    }

    BeastHttpClient::Response BeastHttpClient::put(std::string_view target, std::string_view body) {

        return m_impl->put(target, body);
    }

    BeastHttpClient::Response BeastHttpClient::patch(
        std::string_view target,
        std::string_view body) {

        return m_impl->patch(target, body);
    }

    BeastHttpClient::Response BeastHttpClient::del(std::string_view target) {
        return m_impl->del(target);
    }

    BeastHttpClient::Response BeastHttpClient::head(std::string_view target) {
        return m_impl->head(target);
    }
}
