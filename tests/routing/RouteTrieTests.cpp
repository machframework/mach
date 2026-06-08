#include <cassert>
#include <iostream>

#include "routing/RouteTrie.hpp"

constexpr const char* GREEN = "\033[32m";
constexpr const char* RED = "\033[31m";
constexpr const char* RESET = "\033[0m";

void fail(
	std::string_view testName,
	mach::detail::routing::RouteMatchStatus expected,
	mach::detail::routing::RouteMatchStatus actual);

int main() {

	mach::detail::routing::RouteTrie t;

	auto endpoint1 = mach::detail::routing::Endpoint();
	t.addRoute({ "users" }, mach::http::Method::Get, &endpoint1);

	auto endpoint2 = mach::detail::routing::Endpoint();
	t.addRoute({ "users", "names" }, mach::http::Method::Get, &endpoint2);

	auto endpoint3 = mach::detail::routing::Endpoint();
	t.addRoute({ "users", "names", "desc" }, mach::http::Method::Get, &endpoint3);

	auto endpoint4 = mach::detail::routing::Endpoint();
	t.addRoute({ "users" }, mach::http::Method::Post, &endpoint4);

	auto endpoint5 = mach::detail::routing::Endpoint();
	t.addRoute({ "api", "v1", "users" }, mach::http::Method::Get, &endpoint5);

	auto endpoint6 = mach::detail::routing::Endpoint();
	t.addRoute({ "api", "v1", "posts" }, mach::http::Method::Get, &endpoint6);

	auto endpoint7 = mach::detail::routing::Endpoint();
	t.addRoute({ "health" }, mach::http::Method::Get, &endpoint7);

	auto endpoint8 = mach::detail::routing::Endpoint();
	t.addRoute({ "api", "v1", "users" }, mach::http::Method::Delete, &endpoint8);

	// -------------------------
	// Double route definition tests
	// -------------------------

	try {
		auto endpoint9 = mach::detail::routing::Endpoint();

		t.addRoute(
			{ "api", "v1", "users" },
			mach::http::Method::Delete,
			&endpoint9);

		std::cerr << RED
			<< "[FAIL] Expected std::logic_error"
			<< RESET
			<< std::endl;

		return 1;
	}
	catch (const std::logic_error& e) {
		std::cout << GREEN
			<< "[SUCCESS] Duplicate route insertion threw std::logic_error: "
			<< e.what()
			<< RESET
			<< std::endl;
	}
	catch (...) {
		std::cerr << RED
			<< "[FAIL] Wrong exception type thrown"
			<< RESET
			<< std::endl;

		return 1;
	}

	std::cout << GREEN << "[SUCCESS] Duplicate route tests passed!" << RESET << std::endl;

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

		if (match.status != mach::detail::routing::RouteMatchStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match GET /users/names";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users", "names" });

		if (match.status != mach::detail::routing::RouteMatchStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match GET /users/names/desc";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users", "names", "desc" });

		if (match.status != mach::detail::routing::RouteMatchStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::Found,
				match.status);

			return 1;
		}
	}

	{ // Method not allowed instead of Found
		constexpr auto testName = "Match POST /users";

		auto match = t.matchRoute(
			mach::http::Method::Post,
			{ "users" });

		if (match.status != mach::detail::routing::RouteMatchStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match GET /api/v1/users";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "api", "v1", "users" });

		if (match.status != mach::detail::routing::RouteMatchStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match DELETE /api/v1/users";

		auto match = t.matchRoute(
			mach::http::Method::Delete,
			{ "api", "v1", "users" });

		if (match.status != mach::detail::routing::RouteMatchStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::Found,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Match GET /health";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "health" });

		if (match.status != mach::detail::routing::RouteMatchStatus::Found) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::Found,
				match.status);

			return 1;
		}
	}

	std::cout << GREEN << "[SUCCESS] Found route tests passed!" << RESET << std::endl;

	// -------------------------
	// Not Found tests
	// -------------------------

	{
		constexpr auto testName = "Unknown route GET /orders";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "orders" });

		if (match.status != mach::detail::routing::RouteMatchStatus::NotFound) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::NotFound,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Unknown route GET /users/emails";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users", "emails" });

		if (match.status != mach::detail::routing::RouteMatchStatus::NotFound) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::NotFound,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Unknown route GET /api/v2/users";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "api", "v2", "users" });

		if (match.status != mach::detail::routing::RouteMatchStatus::NotFound) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::NotFound,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Unknown route GET /users/names/asc";

		auto match = t.matchRoute(
			mach::http::Method::Get,
			{ "users", "names", "asc" });

		if (match.status != mach::detail::routing::RouteMatchStatus::NotFound) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::NotFound,
				match.status);

			return 1;
		}
	}

	std::cout << GREEN << "[SUCCESS] Not Found tests passed!" << RESET << std::endl;

	// -------------------------
	// Method Not Allowed tests
	// -------------------------

	{
		constexpr auto testName = "Method not allowed PUT /users";

		auto match = t.matchRoute(
			mach::http::Method::Put,
			{ "users" });

		if (match.status != mach::detail::routing::RouteMatchStatus::MethodNotAllowed) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::MethodNotAllowed,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Method not allowed DELETE /users/names";

		auto match = t.matchRoute(
			mach::http::Method::Delete,
			{ "users", "names" });

		if (match.status != mach::detail::routing::RouteMatchStatus::MethodNotAllowed) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::MethodNotAllowed,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Method not allowed POST /api/v1/posts";

		auto match = t.matchRoute(
			mach::http::Method::Post,
			{ "api", "v1", "posts" });

		if (match.status != mach::detail::routing::RouteMatchStatus::MethodNotAllowed) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::MethodNotAllowed,
				match.status);

			return 1;
		}
	}

	{
		constexpr auto testName = "Method not allowed HEAD /health";

		auto match = t.matchRoute(
			mach::http::Method::Head,
			{ "health" });

		if (match.status != mach::detail::routing::RouteMatchStatus::MethodNotAllowed) {
			fail(
				testName,
				mach::detail::routing::RouteMatchStatus::MethodNotAllowed,
				match.status);

			return 1;
		}
	}

	std::cout << GREEN << "[SUCCESS] Method Not Allowed tests passed!" << RESET << std::endl;

	std::cout << GREEN << "[SUCCESS] All tests passed!" << RESET << std::endl;
	return 0;
}

void fail(
	std::string_view testName,
	mach::detail::routing::RouteMatchStatus expected,
	mach::detail::routing::RouteMatchStatus actual)
{
	std::cerr
		<< RED
		<< "[FAIL] "
		<< testName
		<< ": status should've been '"
		<< mach::detail::routing::toString(expected)
		<< "' but is '"
		<< mach::detail::routing::toString(actual)
		<< "'"
		<< RESET
		<< std::endl;
}