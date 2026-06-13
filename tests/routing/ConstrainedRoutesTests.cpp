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

	auto expectInvalidArgument = [](std::string_view testName, auto&& action) {
		try {
			action();

			std::cout
				<< test::RED
				<< "[FAIL] " << testName << " - expected std::invalid_argument but nothing was thrown"
				<< test::RESET
				<< std::endl;
		}
		catch (const std::invalid_argument&) {
			std::cout
				<< test::GREEN
				<< "[SUCCESS] " << testName << " passed!"
				<< test::RESET
				<< std::endl;
		}
		catch (const std::exception& ex) {
			std::cout
				<< test::RED
				<< "[FAIL] " << testName << " - expected std::invalid_argument but got: "
				<< ex.what()
				<< test::RESET
				<< std::endl;
		}
		catch (...) {
			std::cout
				<< test::RED
				<< "[FAIL] " << testName << " - expected std::invalid_argument but got unknown exception"
				<< test::RESET
				<< std::endl;
		}
		};

	auto expectNoThrow = [](std::string_view testName, auto&& action) {
		try {
			action();

			std::cout
				<< test::GREEN
				<< "[SUCCESS] " << testName << " passed!"
				<< test::RESET
				<< std::endl;
		}
		catch (const std::exception& ex) {
			std::cout
				<< test::RED
				<< "[FAIL] " << testName << " - unexpected exception: "
				<< ex.what()
				<< test::RESET
				<< std::endl;
		}
		catch (...) {
			std::cout
				<< test::RED
				<< "[FAIL] " << testName << " - unknown unexpected exception"
				<< test::RESET
				<< std::endl;
		}
		};

	// Invalid constraint names
	expectInvalidArgument("Reject unknown constraint type", [&] {
		app.get("/users/{name:banana}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject empty constraint after colon", [&] {
		app.get("/users/{name:}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject constraint with spaces", [&] {
		app.get("/users/{name: int}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject constraint with extra colon", [&] {
		app.get("/users/{name:int:banana}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject constraint with weird characters", [&] {
		app.get("/users/{name:i#nt}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	// Malformed parameter syntax
	expectInvalidArgument("Reject empty parameter name", [&] {
		app.get("/users/{:int}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject duplicate parameter names", [&] {
		app.get("/users/{id:int}/posts/{id:int}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject nested braces", [&] {
		app.get("/users/{{id:int}}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	expectInvalidArgument("Reject mixed nested braces", [&] {
		app.get("/users/{id:{int}}", [](mach::Context&) {
			std::cout << "Shouldn't be reached" << std::endl;
			});
		});

	// Valid registrations
	expectNoThrow("Register explicit string constraint", [&] {
		app.get("/users/{name:string}/{age:int}", [](mach::Context& context) {
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
					<< test::GREEN
					<< "[SUCCESS] Multiple constrained route parameter extraction passed!"
					<< test::RESET
					<< std::endl;
			}
			});
		});

	expectNoThrow("Register implicit string parameter", [&] {
		app.get("/posts/{slug}", [](mach::Context& context) {
			constexpr auto testName = "Extract implicit string parameter";

			if (requireEqual(
				testName,
				"hello-world",
				context.request.routeParam("slug"),
				"route parameter 'slug'"
			)) {
				std::cout
					<< test::GREEN
					<< "[SUCCESS] Implicit string route parameter extraction passed!"
					<< test::RESET
					<< std::endl;
			}
			});
		});

	expectNoThrow("Register int-only route", [&] {
		app.get("/orders/{orderId:int}", [](mach::Context& context) {
			constexpr auto testName = "Extract constrained int parameter";

			if (requireEqual(
				testName,
				"123",
				context.request.routeParam("orderId"),
				"route parameter 'orderId'"
			)) {
				std::cout
					<< test::GREEN
					<< "[SUCCESS] Constrained int route parameter extraction passed!"
					<< test::RESET
					<< std::endl;
			}
		});
	});

	expectNoThrow("Register static route competing with constrained param route", [&] {
		app.get("/orders/latest", [](mach::Context&) {
			std::cout
				<< test::GREEN
				<< "[SUCCESS] Static route precedence over constrained parameter route passed!"
				<< test::RESET
				<< std::endl;
			});
		});

	std::cout
		<< test::GREEN
		<< "[SUCCESS] Route parameter registration torture tests completed!"
		<< test::RESET
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
