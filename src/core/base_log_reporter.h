#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	/**
	 * Base class for things that report log information to a consumer.
	 * This consumer can choose to do what it likes with the messages.
	 */
	struct base_log_reporter {
		typedef std::function<void(const std::string&, LogSeverity)> LogFunction;

		inline void set_progress(LogFunction func) {
			logCallback = func; 
		}

		inline void Log(const std::string& message, LogSeverity type) {
			if(logCallback)
				logCallback(message, type);
		}

		/**
		 * Use a progress callback from a already bound progress reporter object.
		 */
		inline void forward(base_log_reporter& lastChain) {
			if(lastChain.logCallback)
				set_progress(lastChain.logCallback);
		}

	protected:
		LogFunction logCallback;
	};


#define VARARGS_LOG(type, ...) { std::stringstream ss; ss << __VA_ARGS__; Log(ss.str(), type); ss.clear(); }


}
}