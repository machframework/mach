#include <mach/App.hpp>
#include <mach/Context.hpp>

#include "Testing.hpp"

#include <iostream>

int main()
{
	auto app = mach::App("127.0.0.1", 3143, test::THREADS);

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
	{
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
				"user id: " +
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
		<< test::GREEN
		<< "[INFO] Routing runtime test server running on http://127.0.0.1:3143"
		<< test::RESET
		<< std::endl;

	app.run();

	return 0;
}