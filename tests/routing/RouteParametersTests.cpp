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

bool testDuplicateParameterNamesAreRejected()
{
	constexpr auto testName = "Reject duplicate route parameter names";

	auto app = mach::App("127.0.0.1", 3143, threads);

	try {
		app.addRoute(
			mach::http::Method::Get,
			"/users/{age}/{age}",
			[](mach::Context&) {}
		);

		fail(testName, "expected std::invalid_argument, but no exception was thrown");
		return false;
	}
	catch (const std::invalid_argument& e) {
		std::cout
			<< test::GREEN
			<< "[SUCCESS] "
			<< testName
			<< " threw std::invalid_argument: "
			<< e.what()
			<< test::RESET
			<< std::endl;

		return true;
	}
	catch (const std::exception& e) {
		std::cerr
			<< test::RED
			<< "[FAIL] "
			<< testName
			<< ": expected std::invalid_argument, but got different std::exception: "
			<< e.what()
			<< test::RESET
			<< std::endl;

		return false;
	}
	catch (...) {
		fail(testName, "expected std::invalid_argument, but got unknown exception type");
		return false;
	}
}

int main()
{
	if (!testDuplicateParameterNamesAreRejected()) {
		return 1;
	}

	auto app = mach::App("127.0.0.1", 3143, threads);

	app.addRoute(
		mach::http::Method::Get,
		"/users/{name}/{age}",
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

	app.addRoute(
		mach::http::Method::Get,
		"/users/{name}",
		[](mach::Context& context) {
			constexpr auto testName = "Extract single route parameter";

			(void)requireEqual(
				testName,
				"Asaf",
				context.request.routeParam("name"),
				"route parameter 'name'"
			);
		}
	);

	app.addRoute(
		mach::http::Method::Get,
		"/users/me",
		[](mach::Context&) {
			std::cout
				<< test::GREEN
				<< "[SUCCESS] Static route precedence: /users/me hit static route"
				<< test::RESET
				<< std::endl;
		}
	);

	std::cout
		<< test::GREEN
		<< "[SUCCESS] Route parameter registration tests passed!"
		<< test::RESET
		<< std::endl;

	std::cout
		<< "Manual Postman checks:\n"
		<< "  GET http://127.0.0.1:3143/users/Asaf\n"
		<< "  GET http://127.0.0.1:3143/users/Asaf/16\n"
		<< "  GET http://127.0.0.1:3143/users/me\n";
		
	app.run();

	return 0;
}
