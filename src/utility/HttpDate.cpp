#include "HttpDate.hpp"

#include <array>
#include <chrono>
#include <format>
#include <stdexcept>
#include <string_view>

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
        using namespace std::chrono;

        const auto wholeSeconds = floor<seconds>(time);
        const auto day = floor<days>(wholeSeconds);

        if (day < sys_days{year{0} / January / 1} || day > sys_days{year{9999} / December / 31}) {
            throw std::invalid_argument("HTTP date is outside the supported range");
        }

        const year_month_day date{day};
        const weekday weekDay{day};
        const hh_mm_ss timeOfDay{wholeSeconds - day};

        return std::format(
            "{}, {:02} {} {:04} {:02}:{:02}:{:02} GMT",
            Weekdays[weekDay.c_encoding()],
            static_cast<unsigned>(date.day()),
            Months[static_cast<unsigned>(date.month()) - 1],
            static_cast<int>(date.year()),
            timeOfDay.hours().count(),
            timeOfDay.minutes().count(),
            timeOfDay.seconds().count());
    }
}
