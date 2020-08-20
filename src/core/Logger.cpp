#include <core.h>
#include <Logger.h>

namespace xb2at {
	namespace core {

		// Implementation of core library logger static members.

		/**
		 * The output function that the core logger will use across all channels.
		 * If null, then the core logger will not log anything.
		 */
		std::function<void(const std::string, LogSeverity)> Logger::OutputFunction;

		/**
		 * Whether or not verbose/debug messages should be allowed across all channels.
		 */
		bool Logger::AllowVerbose = false;

	} // namespace core
} // namespace xb2at