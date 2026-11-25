 #pragma once

 #include <optional>
 #include <string_view>

 namespace mach::detail::http
{
     [[nodiscard]] std::optional<std::string_view> extractMediaType(
         std::string_view contentType) noexcept;

     [[nodiscard]] std::optional<std::string_view> extractCharset(
         std::string_view contentType) noexcept;

     [[nodiscard]] bool matchesMediaType(
         std::string_view contentType,
         std::string_view expectedMediaType) noexcept;

     [[nodiscard]] bool isJsonMediaType(std::string_view contentType) noexcept;

     [[nodiscard]] bool hasUnsupportedJsonCharset(std::string_view contentType) noexcept;
 }
