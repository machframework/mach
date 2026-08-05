#include "HttpDate.hpp"

#include <array>

namespace
{
    constexpr std::array<std::string_view, 7>
        Weekdays{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    constexpr std::array<std::string_view, 12>
        Months{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
}

namespace mach::detail
{
    std::string formatHttpDate(std::chrono::system_clock::time_point time) {
        const std::time_t t = std::chrono::system_clock::to_time_t(time);

        std::tm utc{};

#ifdef _WIN32
        gmtime_s(&utc, &t);
#else
        gmtime_r(&t, &utc);
#endif

        return std::format(
            "{}, {:02} {} {:04} {:02}:{:02}:{:02} GMT",
            Weekdays[utc.tm_wday],
            utc.tm_mday,
            Months[utc.tm_mon],
            utc.tm_year + 1900,
            utc.tm_hour,
            utc.tm_min,
            utc.tm_sec);
    }
}
