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
	{ }

private:
	Db m_userDb;
};

namespace di = mach::detail::di;

int main() {
	di::Container container;

	container.addService<Logger>(di::ServiceLifetime::Scoped);
	//container.addService<Db, Logger>(di::ServiceLifetime::Scoped);
	container.addService<UserService, Db>(di::ServiceLifetime::Scoped);

	try {
		container.addService<Logger>(di::ServiceLifetime::Scoped);
	}
	catch (const std::logic_error& ex) {
		std::cout << "Error: " << ex.what() << std::endl;
	}

	auto scope = container.createScope();

	auto& logger = scope.resolve<Logger>();
	logger.Log("Logging from a dynamically resolved logger!");

	// missing dependency
	try {
		auto& userService = scope.resolve<UserService>();
	}
	catch (const std::logic_error& ex) {
		std::cout << "Error: " << ex.what() << std::endl;
	}
}
