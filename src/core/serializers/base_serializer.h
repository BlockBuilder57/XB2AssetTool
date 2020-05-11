#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	struct base_serializer {
		enum ProgressType : int16 {
			Error,
			Warning,
			Info,
			Verbose
		};
		typedef std::function<void(const std::string&, ProgressType)> progressFunc;

		inline void set_progress(progressFunc func) {
			progressCallback = func; 
		}

		inline void CheckedProgressUpdate(const std::string& message, ProgressType type) {
			if(progressCallback)
				progressCallback(message, type);
		}

	protected:
		progressFunc progressCallback;
	};

}
}