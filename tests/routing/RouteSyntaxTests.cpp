#include <stdexcept>

#include <mach/App.hpp>
#include <mach/Context.hpp>

#include "Testing.hpp"

int main() {
	auto app = mach::App("127.0.0.1", 3143, testing::THREADS);

	// -------------------------
	// Missing leading '/'
	// -------------------------
	{
		const std::string testName = "Missing leading /";

		std::cout << "TEST: " << testName << "\n";

		try {
			app.get("users/", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown! Missing leading / tests passed!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}
	}
	
	// -------------------------
	// Route contains '#' or '?'
	// -------------------------
	{
		const std::string testName = "Invalid characters";

		std::cout << "TEST: " << testName << "\n";

		try {
			app.get("/user#s", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/use?rs", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/users#", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		std::cout
			<< testing::GREEN
			<< testName
			<< " tests passed!"
			<< testing::RESET
			<< "\n";
	}

	// -------------------------
	// Unbalanced braces
	// -------------------------
	{
		const std::string testName = "Unbalanced braces";

		std::cout << "TEST: " << testName << "\n";

		try {
			app.get("/{users", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/users}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/{users}}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/{users}{}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/{users}name", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		std::cout
			<< testing::GREEN
			<< testName
			<< " tests passed!"
			<< testing::RESET
			<< "\n";
	}

	// -------------------------
	// Empty parameter/constraint
	// -------------------------
	{
		const std::string testName = "Empty parameter/constraint";

		std::cout << "TEST: " << testName << "\n";

		try {
			app.get("/{}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/{name:}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/{:int}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		std::cout
			<< testing::GREEN
			<< testName
			<< " tests passed!"
			<< testing::RESET
			<< "\n";
	}

	// -------------------------
	// Duplicate parameter names
	// -------------------------
	{
		const std::string testName = "Duplicate parameter names";

		std::cout << "TEST: " << testName << "\n";

		try {
			app.get("/{name}/{name}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown! Missing leading / tests passed!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/{name:int}/{name}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown! Missing leading / tests passed!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		std::cout
			<< testing::GREEN
			<< testName
			<< " tests passed!"
			<< testing::RESET
			<< "\n";
	}

	// -------------------------
	// Unknown constraints
	// -------------------------
	{
		const std::string testName = "Unknown constraints";

		std::cout << "TEST: " << testName << "\n";

		try {
			app.get("/{name:banana}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown! Missing leading / tests passed!"
				<< testing::RESET
				<< std::endl;
		}
		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		try {
			app.get("/{name:integer}", [](mach::Context& context) {
				// shouldn't be reached
				});
		}
		catch (const std::invalid_argument& ex) {
			std::cout
				<< testing::GREEN
				<< "[SUCCESS] invalid_argument exception thrown! Missing leading / tests passed!"
				<< testing::RESET
				<< std::endl;
		}

		catch (...) {
			testing::fail(testName, "An unknown exception was thrown");
			return 1;
		}

		std::cout
			<< testing::GREEN
			<< testName
			<< " tests passed!"
			<< testing::RESET
			<< "\n";
	}

	std::cout
		<< testing::GREEN
		<< "\nRoute syntax tests passed!"
		<< testing::RESET
		<< "\n";

	return 0;
}
