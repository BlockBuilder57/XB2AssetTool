#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	/**
	 * Basic progress reporter.
	 */
	struct base_progress_reporter {
		typedef std::function<void(const std::string&, ProgressType)> progressFunc;

		inline void set_progress(progressFunc func) {
			progressCallback = func; 
		}

		inline void CheckedProgressUpdate(const std::string& message, ProgressType type) {
			if(progressCallback)
				progressCallback(message, type);
		}

		/**
		 * Use a progress callback from a already bound progress reporter object.
		 */
		inline void forward(base_progress_reporter& lastChain) {
			set_progress(lastChain.progressCallback);
		}

	protected:
		progressFunc progressCallback;
	};


#define PROGRESS_UPDATE(type, ...) { std::stringstream ss; ss << __VA_ARGS__; CheckedProgressUpdate(ss.str(), type); ss.clear(); }

#define PROGRESS_UPDATE_MAIN(type, finish, ...) { std::stringstream ss; ss << __VA_ARGS__; ProgressFunction(ss.str(), type, finish); ss.clear(); }

}
}