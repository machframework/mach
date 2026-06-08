#include <cassert>
#include <iostream>

#include "routing/RouteTrie.hpp"

constexpr const char* GREEN = "\033[32m";
constexpr const char* RED = "\033[31m";
constexpr const char* RESET = "\033[0m";

int main() {

	mach::detail::routing::RouteTrie t;

	std::vector<std::string> segments = { "users", "names", "desc" };
	auto method = mach::http::Method::Get;
	auto endpoint = mach::detail::routing::Endpoint();

	t.addRoute(std::move(segments), method, &endpoint);
	t.debugDump();

	/*** FOUND ***/
	auto match = t.matchRoute(mach::http::Method::Get, { "users", "names", "desc" });
	if (match.status != mach::detail::routing::RouteMatchStatus::Found) {
		std::cerr << RED << "[FAIL] Status should've been '"
			<< mach::detail::routing::toString(mach::detail::routing::RouteMatchStatus::Found)
			<< "' but is '" << mach::detail::routing::toString(match.status) << "'" << RESET << std::endl;

		return 1;
	}

	std::cout << GREEN << "[SUCCESS] Found tests passed!" << RESET << std::endl;

	/*** NOT FOUND ***/
	match = t.matchRoute(mach::http::Method::Get, { "users", "names" });
	if (match.status != mach::detail::routing::RouteMatchStatus::NotFound) {
		std::cerr << RED << "[FAIL] Status should've been '"
			<< mach::detail::routing::toString(mach::detail::routing::RouteMatchStatus::NotFound)
			<< "' but is '" << mach::detail::routing::toString(match.status) << "'" << RESET << std::endl;

		return 1;
	}

	match = t.matchRoute(mach::http::Method::Get, { "users", "names", "desc", "ids" });
	if (match.status != mach::detail::routing::RouteMatchStatus::NotFound) {
		std::cerr << RED << "[FAIL] Status should've been '"
			<< mach::detail::routing::toString(mach::detail::routing::RouteMatchStatus::NotFound)
			<< "' but is '" << mach::detail::routing::toString(match.status) << "'" << RESET << std::endl;

		return 1;
	}

	match = t.matchRoute(mach::http::Method::Get, { });
	if (match.status != mach::detail::routing::RouteMatchStatus::NotFound) {
		std::cerr << RED <<  "[FAIL] Status should've been '"
			<< mach::detail::routing::toString(mach::detail::routing::RouteMatchStatus::NotFound)
			<< "' but is '" << mach::detail::routing::toString(match.status) << "'" << RESET << std::endl;

		return 1;
	}

	std::cout << GREEN << "[SUCCESS] Not Found tests passed!" << RESET << std::endl;

	/*** METHOD NOT ALLOWED ***/
	match = t.matchRoute(mach::http::Method::Post, { "users", "names", "desc" });
	if (match.status != mach::detail::routing::RouteMatchStatus::MethodNotAllowed) {
		std::cerr << RED << "[FAIL] Status should've been '"
			<< mach::detail::routing::toString(mach::detail::routing::RouteMatchStatus::MethodNotAllowed)
			<< "' but is '" << mach::detail::routing::toString(match.status) << "'" << RESET << std::endl;

		return 1;
	}

	std::cout << GREEN << "[SUCCESS] Method Not Allowed tests passed!" << RESET << std::endl;

	std::cout << "[SUCCESS] All tests passed!" << RESET << std::endl;

	return 0;
}
