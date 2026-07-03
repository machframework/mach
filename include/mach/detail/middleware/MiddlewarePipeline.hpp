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
		MiddlewarePipeline() {
			s_createdMiddlewares++;
			s_aliveMiddlewares++;
		}

		~MiddlewarePipeline() {
			s_aliveMiddlewares--;
		}

		static std::int64_t aliveCount() {
			return s_aliveMiddlewares.load();
		}

		static std::int64_t createdCount() {
			return s_createdMiddlewares.load();
		}

		MiddlewarePipeline(const MiddlewarePipeline&) = delete;
		MiddlewarePipeline& operator=(const MiddlewarePipeline&) = delete;

		MiddlewarePipeline(MiddlewarePipeline&&) noexcept = default;
		MiddlewarePipeline& operator=(MiddlewarePipeline&&) noexcept = default;

		template <typename TMiddleware>
		void add();

		void invoke(dispatching::RequestExecution& execution, middleware::Next terminal) const;

	private:
		std::vector<std::unique_ptr<middleware::IMiddlewareInvoker>> m_middlewares;

		static inline std::atomic<std::int64_t> s_createdMiddlewares = 0;
		static inline std::atomic<std::int64_t> s_aliveMiddlewares = 0;
	};

	template <typename TMiddleware>
	void MiddlewarePipeline::add() {
		m_middlewares.emplace_back(
			std::make_unique<MiddlewareInvoker<TMiddleware>>()
		);
	}
}
