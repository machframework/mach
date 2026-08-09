#pragma once

#include <string_view>

#include <mach/Json.hpp>

#include <mach/detail/exceptions/BodyBindingException.hpp>

namespace mach::detail::binding
{
    using exceptions::BodyBindingException;

    class BodyBinder {

    public:
        template <typename T>
        [[nodiscard]] T bind(std::string_view body);
    };

    template <typename T>
    T BodyBinder::bind(std::string_view body) {
        if (body.empty()) {
            throw BodyBindingException("The request body is required.");
        }

        try {
            auto json = mach::Json::parse(body);
            return json.get<T>();
        } catch (const nlohmann::json::parse_error&) {
            throw BodyBindingException("The request body contains invalid JSON.");
        } catch (const nlohmann::json::type_error&) {
            throw BodyBindingException("One or more JSON fields have an invalid type.");
        } catch (const nlohmann::json::out_of_range&) {
            throw BodyBindingException("A required JSON field is missing.");
        } catch (const nlohmann::json::exception&) {
            throw BodyBindingException(
                "The request body could not be bound to the requested type.");
        }
    }
}
