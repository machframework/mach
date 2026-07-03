#include <mach/App.hpp>
#include <mach/Context.hpp>

#include "Testing.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string_view>

int main()
{
	auto app = mach::App("127.0.0.1", 3143, testing::THREADS);

	auto pass = [](std::string_view testName) {
		std::cout
			<< testing::GREEN
			<< "[SUCCESS] "
			<< testName
			<< testing::RESET
			<< std::endl;
		};

	auto expectValid = [&](std::string_view testName, auto&& action) {
		try {
			action();
			pass(testName);
		}
		catch (const std::exception& ex) {
			testing::fail(testName, ex.what());
		}
		catch (...) {
			testing::fail(testName, "Unknown exception thrown");
		}
		};

	auto expectInvalidArgument = [&](std::string_view testName, auto&& action) {
		try {
			action();
			testing::fail(testName, "Expected std::invalid_argument but nothing was thrown");
		}
		catch (const std::invalid_argument&) {
			pass(testName);
		}
		catch (const std::exception& ex) {
			testing::fail(testName, std::string("Expected std::invalid_argument but got: ") + ex.what());
		}
		catch (...) {
			testing::fail(testName, "Expected std::invalid_argument but got unknown exception");
		}
		};

	auto expectLogicError = [&](std::string_view testName, auto&& action) {
		try {
			action();
			testing::fail(testName, "Expected std::logic_error but nothing was thrown");
		}
		catch (const std::logic_error&) {
			pass(testName);
		}
		catch (const std::exception& ex) {
			testing::fail(testName, std::string("Expected std::logic_error but got: ") + ex.what());
		}
		catch (...) {
			testing::fail(testName, "Expected std::logic_error but got unknown exception");
		}
		};

	// -------------------------
	// Register root route
	// -------------------------
	{
		constexpr std::string_view testName = "Register root route";

		expectValid(testName, [&] {
			app.get("/", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Register simple static route
	// -------------------------
	{
		constexpr std::string_view testName = "Register simple static route";

		expectValid(testName, [&] {
			app.get("/routing/static", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Register deep static route
	// -------------------------
	{
		constexpr std::string_view testName = "Register deep static route";

		expectValid(testName, [&] {
			app.get("/routing/static/deep/path", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Register simple parameter route
	// -------------------------
	{
		constexpr std::string_view testName = "Register simple parameter route";

		expectValid(testName, [&] {
			app.get("/routing/users/{id}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Register multiple parameter route
	// -------------------------
	{
		constexpr std::string_view testName = "Register multiple parameter route";

		expectValid(testName, [&] {
			app.get("/routing/users/{userId}/posts/{postId}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Register constrained int parameter route
	// -------------------------
	{
		constexpr std::string_view testName = "Register constrained int parameter route";

		expectValid(testName, [&] {
			app.get("/routing/orders/{orderId:int}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Register static route beside parameter route
	// -------------------------
	{
		constexpr std::string_view testName = "Register static route beside parameter route";

		expectValid(testName, [&] {
			app.get("/routing/users/me", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Register same path with different methods
	// -------------------------
	{
		constexpr std::string_view testName = "Register same path with different methods";

		expectValid(testName, [&] {
			app.post("/routing/multi-method", [](mach::Context&) {});
			app.get("/routing/multi-method", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject duplicate exact GET route
	// -------------------------
	{
		constexpr std::string_view testName = "Reject duplicate exact GET route";

		expectLogicError(testName, [&] {
			app.get("/routing/duplicates/exact", [](mach::Context&) {});
			app.get("/routing/duplicates/exact", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject duplicate parameter shape with different names
	// -------------------------
	{
		constexpr std::string_view testName = "Reject duplicate parameter shape with different names";

		expectLogicError(testName, [&] {
			app.get("/routing/duplicates/{id}", [](mach::Context&) {});
			app.get("/routing/duplicates/{name}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject route that does not start with slash
	// -------------------------
	{
		constexpr std::string_view testName = "Reject route that does not start with slash";

		expectInvalidArgument(testName, [&] {
			app.get("routing/no-leading-slash", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject empty route pattern
	// -------------------------
	{
		constexpr std::string_view testName = "Reject empty route pattern";

		expectInvalidArgument(testName, [&] {
			app.get("", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject route containing query marker
	// -------------------------
	{
		constexpr std::string_view testName = "Reject route containing query marker";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/query?x=1", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject route containing fragment marker
	// -------------------------
	{
		constexpr std::string_view testName = "Reject route containing fragment marker";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/fragment#section", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject unclosed parameter brace
	// -------------------------
	{
		constexpr std::string_view testName = "Reject unclosed parameter brace";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject unopened parameter brace
	// -------------------------
	{
		constexpr std::string_view testName = "Reject unopened parameter brace";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/id}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject empty parameter name
	// -------------------------
	{
		constexpr std::string_view testName = "Reject empty parameter name";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject empty constrained parameter name
	// -------------------------
	{
		constexpr std::string_view testName = "Reject empty constrained parameter name";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{:int}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject empty constraint name
	// -------------------------
	{
		constexpr std::string_view testName = "Reject empty constraint name";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id:}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject unknown constraint name
	// -------------------------
	{
		constexpr std::string_view testName = "Reject unknown constraint name";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id:banana}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject duplicate parameter names
	// -------------------------
	{
		constexpr std::string_view testName = "Reject duplicate parameter names";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id}/orders/{id}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject nested parameter braces
	// -------------------------
	{
		constexpr std::string_view testName = "Reject nested parameter braces";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{{id}}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject multiple opening braces in one segment
	// -------------------------
	{
		constexpr std::string_view testName = "Reject multiple opening braces in one segment";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id{name}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject multiple closing braces in one segment
	// -------------------------
	{
		constexpr std::string_view testName = "Reject multiple closing braces in one segment";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id}}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject parameter mixed with static prefix
	// -------------------------
	{
		constexpr std::string_view testName = "Reject parameter mixed with static prefix";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/user-{id}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject parameter mixed with static suffix
	// -------------------------
	{
		constexpr std::string_view testName = "Reject parameter mixed with static suffix";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id}.json", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject parameter with too many colons
	// -------------------------
	{
		constexpr std::string_view testName = "Reject parameter with too many colons";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id:int:extra}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject parameter with whitespace in name
	// -------------------------
	{
		constexpr std::string_view testName = "Reject parameter with whitespace in name";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{user id}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject parameter with whitespace in constraint
	// -------------------------
	{
		constexpr std::string_view testName = "Reject parameter with whitespace in constraint";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/users/{id: int}", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject trailing slash
	// -------------------------
	{
		constexpr std::string_view testName = "Reject trailing slash";

		expectInvalidArgument(testName, [&] {
			app.get("/routing/trailing-slash/", [](mach::Context&) {});
			});
	}

	// -------------------------
	// Reject repeated slash
	// -------------------------
	{
		constexpr std::string_view testName = "Reject repeated slash";

		expectInvalidArgument(testName, [&] {
			app.get("/routing//double-slash", [](mach::Context&) {});
			});
	}

	return 0;
}