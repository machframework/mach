#include "ContentDisposition.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace
{
    constexpr bool isControlCharacter(std::uint32_t codePoint) noexcept {
        return codePoint <= 0x1F || codePoint == 0x7F;
    }

    constexpr bool isContinuationByte(unsigned char byte) noexcept {
        return (byte & 0xC0) == 0x80;
    }

    constexpr bool isAttrCharacter(unsigned char byte) noexcept {
        if ((byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') ||
            (byte >= '0' && byte <= '9')) {
            return true;
        }

        switch (byte) {
        case '!':
        case '#':
        case '$':
        case '&':
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

    std::uint32_t decodeUtf8CodePoint(std::string_view value, std::size_t& position) {
        const auto first = static_cast<unsigned char>(value[position]);

        if (first <= 0x7F) {
            ++position;
            return first;
        }

        std::uint32_t codePoint{};
        std::uint32_t minimum{};
        std::size_t length{};

        if (first >= 0xC2 && first <= 0xDF) {
            codePoint = first & 0x1F;
            minimum = 0x80;
            length = 2;
        } else if (first >= 0xE0 && first <= 0xEF) {
            codePoint = first & 0x0F;
            minimum = 0x800;
            length = 3;
        } else if (first >= 0xF0 && first <= 0xF4) {
            codePoint = first & 0x07;
            minimum = 0x10000;
            length = 4;
        } else {
            throw std::invalid_argument("Filename contains invalid UTF-8");
        }

        if (length > value.size() - position) {
            throw std::invalid_argument("Filename contains invalid UTF-8");
        }

        for (std::size_t i = 1; i < length; ++i) {
            const auto byte = static_cast<unsigned char>(value[position + i]);

            if (!isContinuationByte(byte)) {
                throw std::invalid_argument("Filename contains invalid UTF-8");
            }

            codePoint = (codePoint << 6) | (byte & 0x3F);
        }

        if (codePoint < minimum || codePoint > 0x10FFFF ||
            (codePoint >= 0xD800 && codePoint <= 0xDFFF)) {
            throw std::invalid_argument("Filename contains invalid UTF-8");
        }

        position += length;

        return codePoint;
    }

    void appendExtendedFilename(std::string& result, std::string_view filename) {
        constexpr char hexDigits[] = "0123456789ABCDEF";

        result += "; filename*=UTF-8''";

        for (const auto c : filename) {
            const auto byte = static_cast<unsigned char>(c);

            if (isAttrCharacter(byte)) {
                result += c;
            } else {
                result += '%';
                result += hexDigits[byte >> 4];
                result += hexDigits[byte & 0x0F];
            }
        }
    }
}

namespace mach::detail::http
{
    std::string formatContentDisposition(std::string_view disposition, std::string_view filename) {
        if (filename.empty()) {
            throw std::invalid_argument("Filename cannot be empty");
        }

        if (filename == "." || filename == "..") {
            throw std::invalid_argument("Invalid filename");
        }

        std::string result{disposition};
        result += "; filename=\"";

        bool requiresExtendedFilename = false;
        std::size_t position = 0;

        while (position < filename.size()) {
            const auto codePoint = decodeUtf8CodePoint(filename, position);

            if (isControlCharacter(codePoint)) {
                throw std::invalid_argument("Filename contains control characters");
            }

            if (codePoint == '/' || codePoint == '\\') {
                throw std::invalid_argument("Filename contains path separators");
            }

            if (codePoint > 0x7F || codePoint == '%') {
                result += '_';
                requiresExtendedFilename = true;
            } else {
                if (codePoint == '"') {
                    result += '\\';
                }

                result += static_cast<char>(codePoint);
            }
        }

        result += '"';

        if (requiresExtendedFilename) {
            appendExtendedFilename(result, filename);
        }

        return result;
    }
}
