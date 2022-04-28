#include <modeco/ThreadPool.h>

namespace xb2at::core {

	/**
	 * Asynchronous executor.
	 */
	struct AsyncExecutor {

		/**
		 * Executes a function asynchronously.
		 * 
		 * \return A future that can be used to await the given function
		 */
		template<class F, typename... Args>
		decltype(auto) ExecuteAsyncTask(F&& fun, Args... args) {
			return pool.AddTask(fun, args...);
		}

		/**
		 * Thread pool to use.
		 */
		mco::ThreadPool pool;
	};

} // namespace xb2at::core