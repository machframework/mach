#pragma once

#include <iostream>
#include <thread>

namespace test
{
    inline constexpr const char* GREEN = "\033[32m";
    inline constexpr const char* RED = "\033[31m";
    inline constexpr const char* RESET = "\033[0m";

    inline unsigned int THREADS = std::thread::hardware_concurrency();

	inline void fail(std::string_view testName, std::string_view message)
	{
		std::cerr
			<< test::RED
			<< "[FAIL] "
			<< testName
			<< ": "
			<< message
			<< test::RESET
			<< std::endl;
	}
}