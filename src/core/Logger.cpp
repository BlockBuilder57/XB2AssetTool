#include <core.h>
#include <Logger.h>

namespace xb2at {
namespace core {
	
	std::function<void(const std::string, LogSeverity)> Logger::OutputFunction;

	bool Logger::AllowVerbose = false;

}
}