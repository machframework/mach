#include <string>
#include <unordered_map>

#include <mach/Request.hpp>
#include <mach/Response.hpp>

namespace mach
{
	struct Context {
		Request request;
		Response response;
		std::unordered_map<std::string, std::string> params;
	};
}