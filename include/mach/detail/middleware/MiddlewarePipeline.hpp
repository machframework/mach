#pragma once

#include <memory>
#include <vector>

#include <mach/detail/dispatching/RequestExecution.hpp>
#include <mach/detail/middleware/Next.hpp>
#include <mach/detail/middleware/MiddlewareInvoker.hpp>

namespace mach::detail::middleware
{
	class MiddlewarePipeline {
		
	public:
		MiddlewarePipeline(const MiddlewarePipeline&) = delete;
		MiddlewarePipeline& operator=(const MiddlewarePipeline&) = delete;

		MiddlewarePipeline(MiddlewarePipeline&&) noexcept = default;
		MiddlewarePipeline& operator=(MiddlewarePipeline&&) noexcept = default;

		template <typename TMiddleware>
		void add();

		void invoke(dispatching::RequestExecution& execution, middleware::Next terminal) const;

	private:
		std::vector<std::unique_ptr<middleware::IMiddlewareInvoker>> m_middlewares;
	};

	template <typename TMiddleware>
	void MiddlewarePipeline::add() {
		m_middlewares.emplace_back(
			std::make_unique<MiddlewareInvoker<TMiddleware>>()
		);
	}
}
