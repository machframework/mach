#pragma once

namespace mach::http
{
    /**
     * Represents an HTTP protocol version.
     */
    enum class Version
    {
        Http10,
        Http11,
        Http2,
        Http3,

        /// Represents an unrecognized or unsupported HTTP version.
        Unknown
    };
}
