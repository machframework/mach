#pragma once

#include <string>
#include <unordered_set>

#include <mach/Context.hpp>
#include <mach/Request.hpp>
#include <mach/http/Method.hpp>

#include "Endpoint.hpp"
#include "application/ExecutionPlan.hpp"

namespace mach::detail::routing
{
	using Handler = void(*)(mach::Context&);

	struct RoutingTableEntry {
		Endpoint* endpoint;
		std::unordered_map<std::string, std::string> params;
	};

	class RoutingTable {

	public:

	private:
		std::unordered_set<RoutingTableEntry> m_routes;
	};
}