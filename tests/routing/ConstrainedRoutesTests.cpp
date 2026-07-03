#include <mach/App.hpp>
#include <mach/Context.hpp>
#include <mach/http/Method.hpp>

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

#include "Testing.hpp"

const int threads = static_cast<int>(std::thread::hardware_concurrency());

bool requireEqual(
	std::string_view testName,
	std::string_view expected,
	std::string_view actual,
	std::string_view label)
{
	if (expected != actual) {
		std::cerr
			<< testing::RED
			<< "[FAIL] "
			<< testName
			<< ": "
			<< label
			<< " should've been '"
			<< expected
			<< "' but is '"
			<< actual
			<< "'"
			<< testing::RESET
			<< std::endl;

		return false;
	}

	std::cout
		<< testing::GREEN
		<< "[SUCCESS] "
		<< testName
		<< ": "
		<< label
		<< " is '"
		<< actual
		<< "'"
		<< testing::RESET
		<< std::endl;

	return true;
}

int main()
{
	auto app = mach::App("127.0.0.1", 3143, threads);

	auto expectInvalidArgument = [](std::string_view testName, auto&& action) {
		try {
			action();

			std::cout
				<< testing::RED
				<< "[FAIL] " << testName << " - expected std::invalid_argument but nothing was thrown"
				<< testing::RESET
				<< std::endl;
		}
		catch (const std::invalid_argument&) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] " << testName << " passed!"
				<< testing::RESET
				<< std::endl;
		}
		catch (const std::exception& ex) {
			std::cout
				<< testing::RED
				<< "[FAIL] " << testName << " - expected std::invalid_argument but got: "
				<< ex.what()
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			std::cout
				<< testing::RED
				<< "[FAIL] " << testName << " - expected std::invalid_argument but got unknown exception"
				<< testing::RESET
				<< std::endl;
		}
		};

	auto expectNoThrow = [](std::string_view testName, auto&& action) {
		try {
			action();

			std::cout
				<< testing::GREEN
				<< "[SUCCESS] " << testName << " passed!"
				<< testing::RESET
				<< std::endl;
		}
		catch (const std::exception& ex) {
			std::cout
				<< testing::RED
				<< "[FAIL] " << testName << " - unexpected exception: "
				<< ex.what()
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			std::cout
				<< testing::RED
				<< "[FAIL] " << testName << " - unknown unexpected exception"
				<< testing::RESET
				<< std::endl;
		}
		};

	// Invalid constraint names
	expectInvalidArgument("Reject unknown constraint type", [&] {
		app.mapGet("/users/{name:banana}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject empty constraint after colon", [&] {
		app.mapGet("/users/{name:}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject constraint with spaces", [&] {
		app.mapGet("/users/{name: int}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject constraint with extra colon", [&] {
		app.mapGet("/users/{name:int:banana}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject constraint with weird characters", [&] {
		app.mapGet("/users/{name:i#nt}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	// Malformed parameter syntax
	expectInvalidArgument("Reject empty parameter name", [&] {
		app.mapGet("/users/{:int}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject duplicate parameter names", [&] {
		app.mapGet("/users/{id:int}/posts/{id:int}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject nested braces", [&] {
		app.mapGet("/users/{{id:int}}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject mixed nested braces", [&] {
		app.mapGet("/users/{id:{int}}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	// Valid registrations
	expectNoThrow("Register explicit string constraint", [&] {
		app.mapGet("/users/{name:string}/{age:int}", [](mach::Context& context) {
			constexpr auto testName = "Extract multiple constrained route parameters";

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
					<< testing::GREEN
					<< "[SUCCESS] Multiple constrained route parameter extraction passed!"
					<< testing::RESET
					<< std::endl;
			}
			});
		});

	expectNoThrow("Register implicit string parameter", [&] {
		app.mapGet("/posts/{slug}", [](mach::Context& context) {
			constexpr auto testName = "Extract implicit string parameter";

			if (requireEqual(
				testName,
				"hello-world",
				context.request.routeParam("slug"),
				"route parameter 'slug'"
			)) {
				std::cout
					<< testing::GREEN
					<< "[SUCCESS] Implicit string route parameter extraction passed!"
					<< testing::RESET
					<< std::endl;
			}
			});
		});

	expectNoThrow("Register int-only route", [&] {
		app.mapGet("/orders/{orderId:int}", [](mach::Context& context) {
			constexpr auto testName = "Extract constrained int parameter";

			if (requireEqual(
				testName,
				"123",
				context.request.routeParam("orderId"),
				"route parameter 'orderId'"
			)) {
				std::cout
					<< testing::GREEN
					<< "[SUCCESS] Constrained int route parameter extraction passed!"
					<< testing::RESET
					<< std::endl;
			}
		});
	});

	expectNoThrow("Register static route competing with constrained param route", [&] {
		app.mapGet("/orders/latest", [](mach::Context&) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] Static route precedence over constrained parameter route passed!"
				<< testing::RESET
				<< std::endl;
			});
		});

	std::cout
		<< testing::GREEN
		<< "[SUCCESS] Route parameter registration torture tests completed!"
		<< testing::RESET
		<< std::endl;

	std::cout
		<< "\nManual Postman checks:\n"
		<< "  Should match:\n"
		<< "    GET http://127.0.0.1:3143/users/Asaf/16\n"
		<< "    GET http://127.0.0.1:3143/posts/hello-world\n"
		<< "    GET http://127.0.0.1:3143/orders/123\n"
		<< "    GET http://127.0.0.1:3143/orders/latest\n"
		<< "\n"
		<< "  Should NOT match int-constrained routes:\n"
		<< "    GET http://127.0.0.1:3143/users/Asaf/banana\n"
		<< "    GET http://127.0.0.1:3143/orders/abc\n"
		<< "    GET http://127.0.0.1:3143/orders/12abc\n"
		<< "    GET http://127.0.0.1:3143/orders/-12  // depends whether your int constraint allows negatives\n"
		<< std::endl;

	app.run();

	return 0;
}
