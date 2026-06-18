#include <iostream>
#include <string>

#include "Testing.hpp"

#include "di/Container.hpp"
#include "di/Scope.hpp"
#include "di/ServiceLifetime.hpp"

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
	container.addService<Db, Logger>(di::ServiceLifetime::Scoped);
	container.addService<UserService, Db>(di::ServiceLifetime::Scoped);

	auto scope = container.createScope();

	auto logger = scope.resolve<Logger>();
	logger->Log("Logging from a dynamically resolved logger!");

	auto db = scope.resolve<Db>();
	int result = db->query();
	
	logger->Log("Got dynamic query result " + std::to_string(result));
}
