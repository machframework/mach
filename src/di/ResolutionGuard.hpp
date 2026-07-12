#pragma once

#include <typeindex>
#include <vector>

namespace mach::detail::di
{
	class ResolutionGuard {

	public:
		ResolutionGuard(std::vector<std::type_index>& stack, std::type_index type)
			: m_stack(stack)
		{
			m_stack.push_back(type);
		}

		~ResolutionGuard() {
			m_stack.pop_back();
		}

		ResolutionGuard(const ResolutionGuard&) = delete;
		ResolutionGuard& operator=(const ResolutionGuard&) = delete;
	private:
		std::vector<std::type_index>& m_stack;
	};
}
