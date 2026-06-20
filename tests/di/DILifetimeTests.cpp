#include <iostream>
#include <string>

#include "Testing.hpp"

#include <mach/detail/di/Container.hpp>
#include <mach/detail/di/Scope.hpp>
#include <mach/detail/di/ServiceLifetime.hpp>


class Logger {
public:
	void Log(const std::string& value) {
		std::cout << value << std::endl;
	}
};

class Db {
public:
	Db(Logger logger) {
		m_logger = logger;
	}

	int query() {
		return 40;
	}

private:
	Logger m_logger;
};

class UserService {
public:
	UserService(Db userDb)
		: m_userDb(userDb)
	{
	}

private:
	Db m_userDb;
};

namespace di = mach::detail::di;

int main() {
	// setup
	di::Container container;

	container.addService<Logger>(di::ServiceLifetime::Scoped);

	{
		// same scope, should be the same object
		const std::string testName = "Objects from same scope test";

		auto s = container.createScope();

		auto logger1 = s.resolve<Logger>();
		auto logger2 = s.resolve<Logger>();

		if (logger1 != logger2) {
			test::fail(testName, "Ojbects are supposed to be the same");
			return 1;
		}

		std::cout
			<< test::GREEN
			<< "[SUCCESS] " << testName << " passed!"
			<< test::RESET
			<< std::endl;
	}
	
	{
		// different scopes, should be different objects
		const std::string testName = "Objects from different scopes test";

		auto s1 = container.createScope();
		auto s2 = container.createScope();

		auto logger1 = s1.resolve<Logger>();
		auto logger2 = s2.resolve<Logger>();

		if (logger1 == logger2) {
			test::fail(testName, "Objects were supposed to be different");
			return 1;
		}

		std::cout
			<< test::GREEN
			<< "[SUCCESS] " << testName << " passed!"
			<< test::RESET
			<< std::endl;
	}

	{
		// singleton, same scope, same object
		const std::string testName = "Singleton objects in the same scope test";

		di::Container container;
		container.addService<Logger>(di::ServiceLifetime::Singleton);

		auto s = container.createScope();
		auto logger1 = s.resolve<Logger>();
		auto logger2 = s.resolve<Logger>();

		if (logger1 != logger2) {
			test::fail(testName, "Should've been the same object");
			return 1;
		}

		std::cout
			<< test::GREEN
			<< "[SUCCESS] " << testName << " passed!"
			<< test::RESET
			<< std::endl;
	}

	{
		// singleton, different scopes, same object
		const std::string testName = "Singleton objects in different scopes test";

		di::Container container;
		container.addService<Logger>(di::ServiceLifetime::Singleton);

		auto s1 = container.createScope();
		auto s2 = container.createScope();

		auto logger1 = s1.resolve<Logger>();
		auto logger2 = s2.resolve<Logger>();

		if (logger1 != logger2) {
			test::fail(testName, "Should've been the same object");
			return 1;
		}

		std::cout
			<< test::GREEN
			<< "[SUCCESS] " << testName << " passed!"
			<< test::RESET
			<< std::endl;
	}

	{
		// transient, same scope, different objects
		const std::string testName = "Transient objects in the same scope test";

		di::Container container;
		container.addService<Logger>(di::ServiceLifetime::Transient);

		auto s = container.createScope();

		auto logger1 = s.resolve<Logger>();
		auto logger2 = s.resolve<Logger>();

		if (logger1 == logger2) {
			test::fail(testName, "Should've been different objects");
			return 1;
		}

		std::cout
			<< test::GREEN
			<< "[SUCCESS] " << testName << " passed!"
			<< test::RESET
			<< std::endl;
	}

	{
		// transient, different scopes, different objects
		const std::string testName = "Transient objects in different scopes";

		di::Container container;
		container.addService<Logger>(di::ServiceLifetime::Transient);

		auto s1 = container.createScope();
		auto s2 = container.createScope();

		auto logger1 = s1.resolve<Logger>();
		auto logger2 = s2.resolve<Logger>();

		if (logger1 == logger2) {
			test::fail(testName, "Should've been different objects");
			return 1;
		}

		std::cout
			<< test::GREEN
			<< "[SUCCESS] " << testName << " passed!"
			<< test::RESET
			<< std::endl;
	}
	
	std::cout
		<< test::GREEN
		<< "\n[SUCCESS] DI Lifetime tests passed!"
		<< test::RESET
		<< std::endl;

	return 0;
}
