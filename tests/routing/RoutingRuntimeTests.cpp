#include <mach/App.hpp>
#include <mach/AppBuilder.hpp>
#include <mach/Context.hpp>

#include "Testing.hpp"

#include <iostream>

int main()
{
	auto builder = mach::AppBuilder("127.0.0.1", 3143, testing::THREADS);
	auto app = builder.build();

	// -------------------------
	// Static route
	// -------------------------
	{
		app.get("/runtime/static", [](mach::Context& context) {
			context.response.body("static route reached");
			});
	}

	// -------------------------
	// Static route beats parameter route
	// -------------------------
	{
		app.get("/runtime/precedence/{id}", [](mach::Context& context) {
			context.response.body("parameter route reached");
			});

		app.get("/runtime/precedence/me", [](mach::Context& context) {
			context.response.body("static route reached");
			});
	}

	// -------------------------
	// Parameter extraction
	// -------------------------
	{ // problematic for next one
		app.get("/runtime/users/{id}", [](mach::Context& context) {
			context.response.body(
				"user id: " + std::string(context.request.routeParam("id"))
			);
			});
	}

	// -------------------------
	// Multiple parameter extraction
	// -------------------------
	{
		app.get("/runtime/users/{userId}/posts/{postId}", [](mach::Context& context) {
			context.response.body(
				"user id: 123 " +
				std::string(context.request.routeParam("userId")) +
				", post id: " +
				std::string(context.request.routeParam("postId"))
			);
			});
	}

	// -------------------------
	// Int constraint
	// -------------------------
	{
		app.get("/runtime/orders/{id:int}", [](mach::Context& context) {
			context.response.body(
				"order id: " + std::string(context.request.routeParam("id"))
			);
			});
	}

	// -------------------------
	// MethodNotAllowed route
	// -------------------------
	{
		app.get("/runtime/get-only", [](mach::Context& context) {
			context.response.body("GET route reached");
			});
	}

	// -------------------------
	// POST route
	// -------------------------
	{
		app.post("/runtime/post-route", [](mach::Context& context) {
			context.response.body("POST route reached");
			});
	}

	std::cout
		<< testing::GREEN
		<< "[INFO] Routing runtime test server running on http://127.0.0.1:3143"
		<< testing::RESET
		<< std::endl;

	std::cout
		<< "Waiting for manual checks:\n"
		<< "\n"

		<< "GET  /runtime/static\n"
		<< "=> 200, 'static route reached'\n"
		<< "\n"

		<< "GET  /runtime/precedence/me\n"
		<< "=> 200, 'static route reached'\n"
		<< "\n"

		<< "GET  /runtime/precedence/asaf\n"
		<< "=> 200, 'parameter route reached'\n"
		<< "\n"

		<< "GET  /runtime/users/123\n"
		<< "=> 200, 'user id: 123'\n"
		<< "\n"

		<< "GET  /runtime/users/asaf/posts/42\n"
		<< "=> 200, 'user id: asaf, post id: 42'\n"
		<< "\n"

		<< "GET  /runtime/orders/123\n"
		<< "=> 200, 'order id: 123'\n"
		<< "\n"

		<< "GET  /runtime/orders/abc\n"
		<< "=> 404\n"
		<< "\n"

		<< "GET  /runtime/orders/-123\n"
		<< "=> 200, 'order id: -123'\n"
		<< "\n"

		<< "GET  /runtime/orders/+123\n"
		<< "=> 404\n"
		<< "\n"

		<< "POST /runtime/get-only\n"
		<< "=> 405\n"
		<< "\n"

		<< "POST /runtime/post-route\n"
		<< "=> 200, 'POST route reached'\n"
		<< "\n"

		<< "GET  /runtime/does-not-exist\n"
		<< "=> 404\n"
		<< std::endl;

	app.run();

	return 0;
}