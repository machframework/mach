#pragma once

#include <string_view>

#include <mach/Json.hpp>
#include <mach/Request.hpp>

#include <mach/detail/binding/BindingConcepts.hpp>
#include <mach/detail/core/FixedString.hpp>
#include <mach/detail/exceptions/RequestBindingException.hpp>

namespace mach::detail::binding
{
    using exceptions::RequestBindingException;

    class RequestBinder {

    public:
        template <typename T>
        [[nodiscard]] T bind(std::string_view body);

        template <mach::detail::binding::Binding T>
        [[nodiscard]] T bind(const mach::Request& request);

    private:
        template <typename T>
        [[nodiscard]] T bindBody(const mach::Request& request);

        template <typename T, detail::FixedString Name>
        [[nodiscard]] T bindParam(const mach::Request& request);

        template <typename T, detail::FixedString Name>
        [[nodiscard]] T bindQuery(const mach::Request& request);
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
        // todo: implement
    }

    template <typename T, detail::FixedString Name>
    [[nodiscard]] T RequestBinder::bindParam(const mach::Request& request) {
        // todo: implement
    }

    template <typename T, detail::FixedString Name>
    [[nodiscard]] T RequestBinder::bindQuery(const mach::Request& request) {
        // todo: implement
    }

    template <typename T>
    T RequestBinder::bind(std::string_view body) {
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
}
