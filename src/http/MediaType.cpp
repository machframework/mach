#include <mach/detail/http/MediaType.hpp>

#include <cstddef>

#include "utility/HttpUtils.hpp"

namespace
{
     constexpr char toAsciiLowercase(char c) noexcept {
         if (c >= 'A' && c <= 'Z') {
             return static_cast<char>(c + ('a' - 'A'));
         }

         return c;
     }

     constexpr bool equalsIgnoreCase(std::string_view lhs, std::string_view rhs) noexcept {
         if (lhs.size() != rhs.size()) {
             return false;
         }

         for (std::size_t i = 0; i < lhs.size(); ++i) {
             if (toAsciiLowercase(lhs[i]) != toAsciiLowercase(rhs[i])) {
                 return false;
             }
         }

         return true;
     }

     constexpr bool isTokenCharacter(char c) noexcept {
         if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
             return true;
         }

         switch (c) {
         case '!':
         case '#':
         case '$':
         case '%':
         case '&':
         case '\'':
         case '*':
         case '+':
         case '-':
         case '.':
         case '^':
         case '_':
         case '`':
         case '|':
         case '~':
             return true;

         default:
             return false;
         }
     }

     constexpr bool isToken(std::string_view value) noexcept {
         if (value.empty()) {
             return false;
         }

         for (const auto c : value) {
             if (!isTokenCharacter(c)) {
                 return false;
             }
         }

         return true;
     }

     constexpr bool isValidMediaType(std::string_view value) noexcept {
         const auto slash = value.find('/');

         if (slash == std::string_view::npos) {
             return false;
         }

         const auto type = value.substr(0, slash);
         const auto subtype = value.substr(slash + 1);

         return isToken(type) && isToken(subtype) && type != "*" && subtype != "*";
     }

     constexpr bool isQuotedTextCharacter(char c) noexcept {
         const auto byte = static_cast<unsigned char>(c);

         return byte == '\t' || byte == ' ' || byte == 0x21 || (byte >= 0x23 && byte <= 0x5B) ||
                (byte >= 0x5D && byte <= 0x7E) || byte >= 0x80;
     }

     constexpr bool isQuotedPairCharacter(char c) noexcept {
         const auto byte = static_cast<unsigned char>(c);

         return byte == '\t' || byte == ' ' || (byte >= 0x21 && byte <= 0x7E) || byte >= 0x80;
     }

     enum class CharsetStatus {
         Missing,
         Valid,
         Invalid
     };

     struct CharsetResult {
         CharsetStatus status = CharsetStatus::Missing;
         std::string_view value;
     };

     struct Parameter {
         std::string_view name;
         std::string_view value;
         bool containsEscapes = false;
     };

     bool parseParameter(std::string_view& remaining, Parameter& parameter) noexcept {
         parameter = {};

         remaining = mach::detail::http::trimOws(remaining);

         // Parse the parameter name.
         std::size_t nameLength = 0;

         while (nameLength < remaining.size() && isTokenCharacter(remaining[nameLength])) {
             ++nameLength;
         }

         if (nameLength == 0) {
             return false;
         }

         parameter.name = remaining.substr(0, nameLength);

         remaining.remove_prefix(nameLength);
         remaining = mach::detail::http::trimOws(remaining);

         // Consume the equals sign.
         if (remaining.empty() || remaining.front() != '=') {
             return false;
         }

         remaining.remove_prefix(1);
         remaining = mach::detail::http::trimOws(remaining);

         if (remaining.empty()) {
             return false;
         }

         // Parse a quoted-string or token value.
         if (remaining.front() == '"') {
             remaining.remove_prefix(1);

             std::size_t position = 0;

             while (position < remaining.size()) {
                 const auto c = remaining[position];

                 if (c == '"') {
                     break;
                 }

                 if (c == '\\') {
                     parameter.containsEscapes = true;

                     ++position;

                     if (position == remaining.size() ||
                         !isQuotedPairCharacter(remaining[position])) {
                         return false;
                     }
                 } else if (!isQuotedTextCharacter(c)) {
                     return false;
                 }

                 ++position;
             }

             if (position == remaining.size()) {
                 return false;
             }

             parameter.value = remaining.substr(0, position);

             remaining.remove_prefix(position + 1);
         } else {
             std::size_t valueLength = 0;

             while (valueLength < remaining.size() && isTokenCharacter(remaining[valueLength])) {
                 ++valueLength;
             }

             if (valueLength == 0) {
                 return false;
             }

             parameter.value = remaining.substr(0, valueLength);

             remaining.remove_prefix(valueLength);
         }

         remaining = mach::detail::http::trimOws(remaining);

         return remaining.empty() || remaining.front() == ';';
     }

     CharsetResult parseCharset(std::string_view contentType) noexcept {
         const auto parametersStart = contentType.find(';');

         if (parametersStart == std::string_view::npos) {
             return {};
         }

         auto remaining = contentType.substr(parametersStart);

         CharsetResult result;

         while (!remaining.empty()) {
             if (remaining.front() != ';') {
                 return {CharsetStatus::Invalid, {}};
             }

             remaining.remove_prefix(1);
             remaining = mach::detail::http::trimOws(remaining);

             // Ignore empty parameter segments.
             if (remaining.empty() || remaining.front() == ';') {
                 continue;
             }

             Parameter parameter;

             if (!parseParameter(remaining, parameter)) {
                 return {CharsetStatus::Invalid, {}};
             }

             if (!equalsIgnoreCase(parameter.name, "charset")) {
                 continue;
             }

             // Reject duplicate charset parameters.
             if (result.status != CharsetStatus::Missing) {
                 return {CharsetStatus::Invalid, {}};
             }

             // A charset must be a non-empty token.
             // Escaped values cannot be returned as a decoded string_view.
             if (parameter.containsEscapes || !isToken(parameter.value)) {
                 return {CharsetStatus::Invalid, {}};
             }

             result = {CharsetStatus::Valid, parameter.value};
         }

         return result;
     }
 }

 namespace mach::detail::http
{
     std::optional<std::string_view> extractMediaType(std::string_view contentType) noexcept {
         const auto parametersStart = contentType.find(';');
         const auto mediaType = trimOws(contentType.substr(0, parametersStart));

         if (!isValidMediaType(mediaType)) {
             return std::nullopt;
         }

         return mediaType;
     }

     std::optional<std::string_view> extractCharset(std::string_view contentType) noexcept {
         if (!extractMediaType(contentType)) {
             return std::nullopt;
         }

         const auto result = parseCharset(contentType);

         if (result.status != CharsetStatus::Valid) {
             return std::nullopt;
         }

         return result.value;
     }

     bool matchesMediaType(
         std::string_view contentType,
         std::string_view expectedMediaType) noexcept {
         const auto mediaType = extractMediaType(contentType);

         expectedMediaType = trimOws(expectedMediaType);

         return mediaType.has_value() && isValidMediaType(expectedMediaType) &&
                equalsIgnoreCase(*mediaType, expectedMediaType);
     }

     bool isJsonMediaType(std::string_view contentType) noexcept {
         const auto mediaType = extractMediaType(contentType);

         if (!mediaType) {
             return false;
         }

         if (equalsIgnoreCase(*mediaType, "application/json")) {
             return true;
         }

         constexpr std::string_view suffix = "+json";

         const auto slash = mediaType->find('/');
         const auto subtype = mediaType->substr(slash + 1);

         return subtype.size() > suffix.size() &&
                equalsIgnoreCase(subtype.substr(subtype.size() - suffix.size()), suffix);
     }

     bool hasUnsupportedJsonCharset(std::string_view contentType) noexcept {
         if (!extractMediaType(contentType)) {
             return true;
         }

         const auto result = parseCharset(contentType);

         switch (result.status) {
         case CharsetStatus::Missing:
             return false;

         case CharsetStatus::Valid:
             return !equalsIgnoreCase(result.value, "utf-8");

         case CharsetStatus::Invalid:
             return true;
         }

         return true;
     }
 }
