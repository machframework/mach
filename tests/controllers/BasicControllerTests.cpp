#include <iostream>
#include <string_view>

#include <mach/controllers/ControllerBase.hpp>
#include <mach/Reply.hpp>
#include <mach/AppBuilder.hpp>

#include <mach/diagnostics/TerminateHandler.hpp>

#include "Testing.hpp"

// dependencies
class Logger {

public:
	void log(std::string_view s) {
		std::cout << "Logging: " << s << "\n";
	}
};

class Initiator {

public:
	Initiator() {
		std::cout << "Initiating\n";
	}
};

class DummyDb {

public:
	int getInt() { return 3143; }
	std::string getString() { return "localhost"; }
	unsigned int getUint() { return 12; }
};

class UserRepo {

public:
	UserRepo(Logger logger, DummyDb db)
		: m_logger(logger), m_db(db)
	{ 
		m_logger.log("Starting UserRepo");
	}

	void get(int type, std::string& res) {
		switch (type) {
		case 0: // int
			res = std::to_string(m_db.getInt());
			break;
		
		case 1: // string
			res = m_db.getString();
			break;

		case 2:
			res = std::to_string(m_db.getUint());
			break;

		default:
			res = "Invalid type";
		}
	}

private:
	Logger m_logger;
	DummyDb m_db;
};

class UserService {

public:
	UserService(UserRepo repo, Initiator init, Logger logger)
		: m_repo(repo),
		m_initiator(init),
		m_logger(logger)
	{ }

	std::string serve(int type) {
		std::string res;
		m_repo.get(type, res);
		m_logger.log(res);
		return res;
	}

private:
	UserRepo m_repo;
	Initiator m_initiator;
	Logger m_logger;
};
// dependencies

// controller
class UserController : public mach::ControllerBase {

public:
	static inline std::string route = "/users";

	UserController(UserService userService)
		: m_userService(userService)
	{ }

	[[mach::get("/{type:int}")]]
	mach::Reply<std::string> getByType() {
		int type = std::stoi(std::string(request().routeParam("type")));

		auto res = m_userService.serve(type);
		if (res.size() >= 10) {
			return notFound<std::string>();
		}

		return ok(res);
	}

	static void configure(mach::ControllerBuilder<UserController>& methods) {
		methods.mapGet("/{type:int}", &UserController::getByType);
	}

private:
	UserService m_userService;
};
// controller

int main() {
	mach::installTerminateHandler();

	auto builder = mach::AppBuilder(std::move(testing::serverOptions));

	builder.addSingleton<Logger>();
	builder.addScoped<Initiator>();
	builder.addSingleton<DummyDb>();
	builder.addScoped<UserRepo, Logger, DummyDb>();
	builder.addScoped<UserService, UserRepo, Initiator, Logger>();
	builder.addController<UserController, UserService>();

	auto app = builder.build();
	return app.run();
}
