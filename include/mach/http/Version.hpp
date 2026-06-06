#pragma once

namespace mach::http
{
    enum class Version
    {
        Http10,
        Http11,
        Http2,
        Http3,
        Unknown
    };
}
