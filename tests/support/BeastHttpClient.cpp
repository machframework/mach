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
        Impl(std::string host, std::uint16_t port)
            : host_(std::move(host)), port_(std::to_string(port)), resolver_(ioContext_),
              stream_(ioContext_) {}

        Response get(std::string_view target) {
            ensureConnected();

            http::request<http::empty_body> request{http::verb::get, target, 11};

            request.set(http::field::host, host_);
            request.set(http::field::user_agent, "Mach Test Client");

            http::write(stream_, request);

            beast::flat_buffer buffer;
            http::response<http::string_body> response;

            http::read(stream_, buffer, response);

            return Response{static_cast<int>(response.result_int()), std::move(response.body())};
        }

    private:
        void ensureConnected() {
            if (stream_.socket().is_open()) {
                return;
            }

            const auto endpoints = resolver_.resolve(host_, port_);
            stream_.connect(endpoints);
        }

        std::string host_;
        std::string port_;

        asio::io_context ioContext_;
        tcp::resolver resolver_;
        beast::tcp_stream stream_;
    };

    BeastHttpClient::Response::Response(int statusCode, std::string body)
        : statusCode_(statusCode), body_(std::move(body)) {}

    int BeastHttpClient::Response::statusCode() const noexcept {
        return statusCode_;
    }

    const std::string& BeastHttpClient::Response::body() const noexcept {
        return body_;
    }

    BeastHttpClient::BeastHttpClient(std::string host, std::uint16_t port)
        : impl_(std::make_unique<Impl>(std::move(host), port)) {}

    BeastHttpClient::~BeastHttpClient() = default;

    BeastHttpClient::BeastHttpClient(BeastHttpClient&&) noexcept = default;

    BeastHttpClient& BeastHttpClient::operator=(BeastHttpClient&&) noexcept = default;

    BeastHttpClient::Response BeastHttpClient::get(std::string_view target) {

        return impl_->get(target);
    }
}
