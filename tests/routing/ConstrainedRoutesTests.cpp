#include <mach/App.hpp>
#include <mach/Context.hpp>
#include <mach/http/Method.hpp>

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

#include "Testing.hpp"

const int threads = static_cast<int>(std::thread::hardware_concurrency());

void fail(std::string_view testName, std::string_view message)
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

bool requireEqual(
	std::string_view testName,
	std::string_view expected,
	std::string_view actual,
	std::string_view label)
{
	if (expected != actual) {
		std::cerr
			<< test::RED
			<< "[FAIL] "
			<< testName
			<< ": "
			<< label
			<< " should've been '"
			<< expected
			<< "' but is '"
			<< actual
			<< "'"
			<< test::RESET
			<< std::endl;

		return false;
	}

	std::cout
		<< test::GREEN
		<< "[SUCCESS] "
		<< testName
		<< ": "
		<< label
		<< " is '"
		<< actual
		<< "'"
		<< test::RESET
		<< std::endl;

	return true;
}

int main()
{
	auto app = mach::App("127.0.0.1", 3143, threads);

	app.addRoute(
		mach::http::Method::Get,
		"/users/{name:string}/{age:int}",
		[](mach::Context& context) {
			constexpr auto testName = "Extract multiple route parameters";

			bool nameOk = requireEqual(
				testName,
				"Asaf",
				context.request.routeParam("name"),
				"route parameter 'name'"
			);

			bool ageOk = requireEqual(
				testName,
				"16",
				context.request.routeParam("age"),
				"route parameter 'age'"
			);

			if (nameOk && ageOk) {
				std::cout
					<< test::GREEN
					<< "[SUCCESS] Multiple route parameter extraction passed!"
					<< test::RESET
					<< std::endl;
			}
		}
	);
	std::cout
		<< test::GREEN
		<< "[SUCCESS] Route parameter registration tests passed!"
		<< test::RESET
		<< std::endl;

	std::cout
		<< "Manual Postman checks:\n"
		<< "  GET http://127.0.0.1:3143/users/Asaf/16\n";

	app.run();

	return 0;
}
