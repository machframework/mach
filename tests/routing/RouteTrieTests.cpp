#include <iostream>

#include <mach/detail/routing/RouteTrie.hpp>
#include <mach/detail/routing/RoutingStatus.hpp>
#include "Testing.hpp"

void fail(
	std::string_view testName,
	mach::detail::routing::RoutingStatus expected,
	mach::detail::routing::RoutingStatus actual);

int main() {

	mach::detail::routing::RouteTrie t;

	auto endpoint1 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Get };
	t.addRoute({ "users" }, &endpoint1);

	auto endpoint2 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Get };
	t.addRoute({ "users", "names" }, &endpoint2);

	auto endpoint3 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Get };
	t.addRoute({ "users", "names", "desc" }, &endpoint3);

	auto endpoint4 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Post };
	t.addRoute({ "users" }, &endpoint4);

	auto endpoint5 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Get };
	t.addRoute({ "api", "v1", "users" }, &endpoint5);

	auto endpoint6 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Get };
	t.addRoute({ "api", "v1", "posts" }, &endpoint6);

	auto endpoint7 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Get };
	t.addRoute({ "health" }, &endpoint7);

	auto endpoint8 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Delete };
	t.addRoute({ "api", "v1", "users" }, &endpoint8);

	auto endpoint9 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Get };
	t.addRoute({ "users", "{age}" }, &endpoint9);

	t.debugDump();
	return 0;

	// -------------------------
	// Double route definition tests
	// -------------------------

	try {
		auto endpoint9 = mach::detail::routing::RouteEndpoint{ .method = mach::http::Method::Delete };

		t.addRoute(
			{ "api", "v1", "users" },
			&endpoint9);

		std::cerr << testing::RED
			<< "[FAIL] Expected std::logic_error"
			<< testing::RESET
			<< std::endl;

		return 1;
	}
	catch (const std::logic_error& e) {
		std::cout << testing::GREEN
			<< "[SUCCESS] Duplicate route insertion threw std::logic_error: "
			<< e.what()
			<< testing::RESET
			<< std::endl;
	}
	catch (...) {
		std::cerr << testing::RED
			<< "[FAIL] Wrong exception type thrown"
			<< testing::RESET
			<< std::endl;

		return 1;
	}

	std::cout << testing::GREEN << "[SUCCESS] Duplicate route tests passed!" << testing::RESET << std::endl;

	//t.debugDump();
	//return 0;

	// -------------------------
	// Found tests
	// -------------------------

	{
		constexpr auto testName = "Match GET /users";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users" });

		if (match.status != mach::detail::routing::RoutingStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match GET /users/names";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users", "names" });

		if (match.status != mach::detail::routing::RoutingStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match GET /users/names/desc";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users", "names", "desc" });

		if (match.status != mach::detail::routing::RoutingStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match POST /users";

		auto match = t.matchRoute(
			mach::http::Method::Post,
			{ "users" });

		if (match.status != mach::detail::routing::RoutingStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match GET /api/v1/users";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "api", "v1", "users" });

		if (match.status != mach::detail::routing::RoutingStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match DELETE /api/v1/users";

		auto match = t.matchRoute(
			mach::http::Method::Delete,
			{ "api", "v1", "users" });

		if (match.status != mach::detail::routing::RoutingStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match GET /health";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "health" });

		if (match.status != mach::detail::routing::RoutingStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::Found,
				match.status);

			return 1;
		}
	}

	std::cout << testing::GREEN << "[SUCCESS] Found route tests passed!" << testing::RESET << std::endl;

	// -------------------------
	// Not Found tests
	// -------------------------

	{
		constexpr auto testName = "Unknown route GET /orders";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "orders" });

		if (match.status != mach::detail::routing::RoutingStatus::NotFound) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::NotFound,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Unknown route GET /users/emails";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users", "emails" });

		if (match.status != mach::detail::routing::RoutingStatus::NotFound) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::NotFound,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Unknown route GET /api/v2/users";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "api", "v2", "users" });

		if (match.status != mach::detail::routing::RoutingStatus::NotFound) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::NotFound,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Unknown route GET /users/names/asc";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users", "names", "asc" });

		if (match.status != mach::detail::routing::RoutingStatus::NotFound) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::NotFound,
				match.status);

			return 1;
		}
	}

	std::cout << testing::GREEN << "[SUCCESS] Not Found tests passed!" << testing::RESET << std::endl;

	// -------------------------
	// Method Not Allowed tests
	// -------------------------

	{
		constexpr auto testName = "Method not allowed PUT /users";

		auto match = t.matchRoute(
			mach::http::Method::Put,
			{ "users" });

		if (match.status != mach::detail::routing::RoutingStatus::MethodNotAllowed) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::MethodNotAllowed,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Method not allowed DELETE /users/names";

		auto match = t.matchRoute(
			mach::http::Method::Delete,
			{ "users", "names" });

		if (match.status != mach::detail::routing::RoutingStatus::MethodNotAllowed) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::MethodNotAllowed,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Method not allowed POST /api/v1/posts";

		auto match = t.matchRoute(
			mach::http::Method::Post,
			{ "api", "v1", "posts" });

		if (match.status != mach::detail::routing::RoutingStatus::MethodNotAllowed) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::MethodNotAllowed,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Method not allowed HEAD /health";

		auto match = t.matchRoute(
			mach::http::Method::Head,
			{ "health" });

		if (match.status != mach::detail::routing::RoutingStatus::MethodNotAllowed) {
			fail(
				testName,
				mach::detail::routing::RoutingStatus::MethodNotAllowed,
				match.status);

			return 1;
		}
	}

	std::cout << testing::GREEN << "[SUCCESS] Method Not Allowed tests passed!" << testing::RESET << std::endl;

	std::cout << testing::GREEN << "[SUCCESS] All tests passed!" << testing::RESET << std::endl;
	return 0;
}

void fail(
	std::string_view testName,
	mach::detail::routing::RoutingStatus expected,
	mach::detail::routing::RoutingStatus actual)
{
	std::cerr
		<< testing::RED
		<< "[FAIL] "
		<< testName
		<< ": status should've been '"
		<< mach::detail::routing::toString(expected)
		<< "' but is '"
		<< mach::detail::routing::toString(actual)
		<< "'"
		<< testing::RESET
		<< std::endl;
}