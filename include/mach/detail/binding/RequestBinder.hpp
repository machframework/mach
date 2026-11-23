#pragma once

#include <string_view>

#include <mach/Json.hpp>
#include <mach/Request.hpp>
#include <mach/exceptions/HttpException.hpp>
#include <mach/http/StatusCode.hpp>

#include <mach/detail/binding/BindingConcepts.hpp>
#include <mach/detail/binding/BindingConversion.hpp>
#include <mach/detail/binding/JsonConcepts.hpp>
#include <mach/detail/core/FixedString.hpp>
#include <mach/detail/exceptions/RequestBindingException.hpp>
#include <mach/detail/http/ContentType.hpp>

namespace mach::detail::binding
{
    using exceptions::RequestBindingException;

    class RequestBinder {

    public:
        template <mach::detail::binding::Binding T>
        [[nodiscard]] T bind(const mach::Request& request);

        template <binding::JsonDeserializable T>
        [[nodiscard]] T bind(const mach::Request& request);

    private:
        template <typename T>
        [[nodiscard]] T bindBody(const mach::Request& request);

        template <typename T, detail::FixedString Name>
        [[nodiscard]] T bindParam(const mach::Request& request);

        template <typename T, detail::FixedString Name>
        [[nodiscard]] T bindQuery(const mach::Request& request);

        static void validateJsonContentType(const mach::Request& request);
    };

    template <mach::detail::binding::Binding T>
    [[nodiscard]] T RequestBinder::bind(const mach::Request& request) {
        if constexpr (BindingTraits<T>::source == BindingSource::Body) {
            return T{bindBody<typename BindingTraits<T>::value_type>(request)};
        } else if constexpr (BindingTraits<T>::source == BindingSource::Param) {
            return T{bindParam<typename BindingTraits<T>::value_type, BindingTraits<T>::name>(
                request)};
        } else if constexpr (BindingTraits<T>::source == BindingSource::Query) {
            return T{bindQuery<typename BindingTraits<T>::value_type, BindingTraits<T>::name>(
                request)};
        }
    }

    template <typename T>
    [[nodiscard]] T RequestBinder::bindBody(const mach::Request& request) {
        validateJsonContentType(request);
        return bind<T>(request);
    }

    template <typename T, detail::FixedString Name>
    [[nodiscard]] T RequestBinder::bindParam(const mach::Request& request) {
        try {
            return convertBindingValue<T>(request.routeParam(std::string_view{Name}));
        } catch (const nlohmann::json::exception&) {
            throw RequestBindingException("The route parameter contains invalid JSON.");
        } catch (const std::invalid_argument&) {
            throw RequestBindingException("The route parameter has an invalid value.");
        }
    }

    template <typename T, detail::FixedString Name>
    [[nodiscard]] T RequestBinder::bindQuery(const mach::Request& request) {
        const auto value = request.query(std::string_view{Name});

        if (!value) {
            if constexpr (detail::traits::isOptional_v<T>) {
                return std::nullopt;
            } else {
                throw RequestBindingException("The required query parameter is missing.");
            }
        }

        try {
            return convertBindingValue<T>(*value);
        } catch (const nlohmann::json::exception&) {
            throw RequestBindingException("The query parameter contains invalid JSON.");
        } catch (const std::invalid_argument&) {
            throw RequestBindingException("The query parameter has an invalid value.");
        }
    }

    template <binding::JsonDeserializable T>
    T RequestBinder::bind(const mach::Request& request) {
        validateJsonContentType(request);

        const auto& body = request.body();

        if (body.empty()) {
            throw RequestBindingException("The request body is required.");
        }

        try {
            auto json = mach::Json::parse(body);
            return json.get<T>();
        } catch (const nlohmann::json::parse_error&) {
            throw RequestBindingException("The request body contains invalid JSON.");
        } catch (const nlohmann::json::type_error&) {
            throw RequestBindingException("One or more JSON fields have an invalid type.");
        } catch (const nlohmann::json::out_of_range&) {
            throw RequestBindingException("A required JSON field is missing.");
        } catch (const nlohmann::json::exception&) {
            throw RequestBindingException(
                "The request body could not be bound to the requested type.");
        }
    }

    inline void RequestBinder::validateJsonContentType(const mach::Request& request) {
        if (request.body().empty()) {
            return;
        }

        const auto contentType = request.header("content-type");

        if (!contentType || !http::matchesMediaType(*contentType, "application/json") ||
            http::hasUnsupportedCharset(*contentType)) {
            throw mach::HttpException(
                mach::http::StatusCode::UnsupportedMediaType,
                "Unsupported Media Type: The request body must be JSON.");
        }
    }
}
