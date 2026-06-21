#include <thread>

#include <mach/App.hpp>
#include <mach/Context.hpp>

int main() {
	int threads = std::thread::hardware_concurrency();

	mach::AppBuilder builder("127.0.0.1", 3143, 16);
	auto app = builder.build();

	// root
	app.get("/", [](mach::Context& context) {
		context.response.body("Ok root");
	});

	// static route lookup
	app.get("/runtime/static", [](mach::Context& context) {
		context.response.body("Ok static route");
	});

	// single parameter extraction
	app.get("/runtime/users/{userId:int}", [](mach::Context& context) {
		context.response.body("Ok single param");
	});

	// multiple parameters
	app.get("/runtime/users/{userName}/posts/{postId:int}", [](mach::Context& context) {
		context.response.body("Ok multiple params");
	});

	// static-vs-param precedence path
	app.get("/runtime/precedence/me", [](mach::Context& context) {
		context.response.body("Ok precedence");
	});

	app.get("/runtime/precedence/{userName}", [](mach::Context& context) {
		context.response.body("Reached when name isn't 'me'");
	});

	app.run();
}
