#pragma once

#include <iostream>
#include <thread>

#include <mach/detail/app/ServerOptions.hpp>

namespace testing
{
    inline constexpr const char* GREEN = "\033[32m";
    inline constexpr const char* RED = "\033[31m";
    inline constexpr const char* RESET = "\033[0m";

    inline unsigned int THREADS = std::thread::hardware_concurrency();

	inline auto serverOptions = mach::detail::app::ServerOptions{
		.host = "127.0.0.1",
		.port = 3143,
		.threads = THREADS
	};

	inline void fail(std::string_view testName, std::string_view message) {
		std::cerr
			<< testing::RED
			<< "[FAIL] "
			<< testName
			<< ": "
			<< message
			<< testing::RESET
			<< std::endl;
	}

	inline void success(std::string_view testName) {
		std::cout
			<< testing::GREEN
			<< "[SUCCESS] " << testName << " passed!"
			<< testing::RESET
			<< std::endl;
	}
}
