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

        Response get(std::string_view target, const Headers& headers);

        Response post(std::string_view target, std::string_view body, const Headers& headers);

        Response put(std::string_view target, std::string_view body, const Headers& headers);

        Response patch(std::string_view target, std::string_view body, const Headers& headers);

        Response del(std::string_view target, const Headers& headers);

        Response head(std::string_view target, const Headers& headers);

    private:
        Response request(
            http::verb method,
            std::string_view target,
            std::string_view body,
            const Headers& headers);

        void ensureConnected();

        std::string m_host;
        std::string m_port;

        asio::io_context m_ioContext;
        tcp::resolver m_resolver;
        beast::tcp_stream m_stream;
    };

    BeastHttpClient::Impl::Impl(std::string host, std::uint16_t port)
        : m_host(std::move(host)), m_port(std::to_string(port)), m_resolver(m_ioContext),
          m_stream(m_ioContext) {}

    BeastHttpClient::Response BeastHttpClient::Impl::request(
        http::verb method,
        std::string_view target,
        std::string_view body,
        const Headers& headers) {

        ensureConnected();

        http::request<http::string_body> request{method, target, 11};

        request.set(http::field::host, m_host);
        request.set(http::field::user_agent, "Mach Test Client");

        for (const auto& [name, value] : headers) {
            request.set(name, value);
        }

        request.body() = body;
        request.prepare_payload();

        http::write(m_stream, request);

        beast::flat_buffer buffer;

        if (method == http::verb::head) {
            http::response_parser<http::string_body> parser;
            parser.skip(true);

            http::read(m_stream, buffer, parser);

            auto response = parser.release();

            return Response{static_cast<int>(response.result_int()), std::move(response.body())};
        }

        http::response<http::string_body> response;

        http::read(m_stream, buffer, response);

        return Response{static_cast<int>(response.result_int()), std::move(response.body())};
    }

    BeastHttpClient::Response BeastHttpClient::Impl::get(
        std::string_view target,
        const Headers& headers) {

        return request(http::verb::get, target, "", headers);
    }

    BeastHttpClient::Response BeastHttpClient::Impl::post(
        std::string_view target,
        std::string_view body,
        const Headers& headers) {

        return request(http::verb::post, target, body, headers);
    }

    BeastHttpClient::Response BeastHttpClient::Impl::put(
        std::string_view target,
        std::string_view body,
        const Headers& headers) {

        return request(http::verb::put, target, body, headers);
    }

    BeastHttpClient::Response BeastHttpClient::Impl::patch(
        std::string_view target,
        std::string_view body,
        const Headers& headers) {

        return request(http::verb::patch, target, body, headers);
    }

    BeastHttpClient::Response BeastHttpClient::Impl::del(
        std::string_view target,
        const Headers& headers) {

        return request(http::verb::delete_, target, "", headers);
    }

    BeastHttpClient::Response BeastHttpClient::Impl::head(
        std::string_view target,
        const Headers& headers) {

        return request(http::verb::head, target, "", headers);
    }

    void BeastHttpClient::Impl::ensureConnected() {
        if (m_stream.socket().is_open()) {
            return;
        }

        const auto endpoints = m_resolver.resolve(m_host, m_port);

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

    BeastHttpClient::Response BeastHttpClient::get(
        std::string_view target,
        const Headers& headers) {

        return m_impl->get(target, headers);
    }

    BeastHttpClient::Response BeastHttpClient::post(
        std::string_view target,
        std::string_view body,
        const Headers& headers) {

        return m_impl->post(target, body, headers);
    }

    BeastHttpClient::Response BeastHttpClient::put(
        std::string_view target,
        std::string_view body,
        const Headers& headers) {

        return m_impl->put(target, body, headers);
    }

    BeastHttpClient::Response BeastHttpClient::patch(
        std::string_view target,
        std::string_view body,
        const Headers& headers) {

        return m_impl->patch(target, body, headers);
    }

    BeastHttpClient::Response BeastHttpClient::del(
        std::string_view target,
        const Headers& headers) {

        return m_impl->del(target, headers);
    }

    BeastHttpClient::Response BeastHttpClient::head(
        std::string_view target,
        const Headers& headers) {

        return m_impl->head(target, headers);
    }
}
