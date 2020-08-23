#pragma once
#include <core.h>

#if defined(_WIN32) && defined(_DEBUG)
	extern "C" void __stdcall OutputDebugStringA(const char* message);
#endif

namespace xb2at {
	namespace core {

		/**
		 * Log reporting serverity.
		 */
		enum class LogSeverity : int16 {
			/**
			 * A verbose message. Should only be emitted
			 * if Logger::AllowVerbose == true.
			 */
			Verbose,
			/**
			 * An informational message. This can be used to report information to the user.
			 */
			Info,

			/**
			 * An message that can be used to warn the user of a condition that may be wrong.
			 */
			Warning,

			/**
			 * An message that can be used to warn the user of a condition that is definitely wrong.
			 */
			Error
		};

		/**
		 * The Core Library Logger.
		 * 
		 * Designed for a mix of usability and performance.
		 */
		struct Logger {
			/**
			 * Get a logger with a user-specified channel name.
			 * 
			 * \param[in] channel_name The channel name to use. Can be anything.
			 */
			static Logger CreateChannel(std::string channel_name) {
				Logger l;

				if(channel_name.empty()) {
					// group every single empty channel name into a global channel
					// this is techinically an error, so we probably should complain.
					// However, this works for now.
					l.channel_name = "Global_Channel";
					return l;
				}

				l.channel_name = channel_name;
				return l;
			}

			static std::function<void(const std::string, LogSeverity)> OutputFunction;

			static bool AllowVerbose;

			// Logging functions

			template<typename... Args>
			inline void info(Args... args) {
				if(!Logger::OutputFunction)
					return;

				std::ostringstream ss;

				//ss << TimestampString() << "[" << channel_name << "] " << Stringify(value, args...);
				ss << "[" << channel_name << "] ";

				((ss << std::forward<Args>(args)), ...);

				Logger::OutputFunction(ss.str(), LogSeverity::Info);

				// Also output the resulting string in the debugger
#if defined(_WIN32) && defined(_DEBUG)
				ss << '\n'; // hack
				OutputDebugStringA(ss.str().c_str());
#endif

				ss.clear();
			}

			template<typename... Args>
			inline void warn(Args... args) {
				if(!Logger::OutputFunction)
					return;

				std::ostringstream ss;
				ss << "[" << channel_name << "] ";

				((ss << std::forward<Args>(args)), ...);

				Logger::OutputFunction(ss.str(), LogSeverity::Warning);

				// Also output the resulting string in the debugger
#if defined(_WIN32) && defined(_DEBUG)
				ss << '\n'; // hack
				OutputDebugStringA(ss.str().c_str());
#endif

				ss.clear();
			}

			template<typename... Args>
			inline void error(Args... args) {
				if(!Logger::OutputFunction)
					return;

				std::ostringstream ss;
				ss << "[" << channel_name << "] ";

				((ss << std::forward<Args>(args)), ...);

				Logger::OutputFunction(ss.str(), LogSeverity::Error);

				// Also output the resulting string in the debugger
#if defined(_WIN32) && defined(_DEBUG)
				ss << '\n'; // hack
				OutputDebugStringA(ss.str().c_str());
#endif

				ss.clear();
			}

			template<typename... Args>
			inline void verbose(Args... args) {
				if(!Logger::OutputFunction)
					return;

				std::ostringstream ss;
				ss << "[" << channel_name << "] ";

				// use pack expansion
				((ss << std::forward<Args>(args)), ...);

				// We always output verbose messages to the windows debug console
#if defined(_WIN32) && defined(_DEBUG)
				if(!Logger::AllowVerbose) {
					ss << '\n'; // hack
					OutputDebugStringA(ss.str().c_str());
					ss.clear();
					return;
				} else {
					ss << '\n';
					OutputDebugStringA(ss.str().c_str());
					ss.seekp(-1, std::ostringstream::cur);
					ss << ' ';
				}
#else
				if(!Logger::AllowVerbose)
					return;
#endif
				Logger::OutputFunction(ss.str(), LogSeverity::Verbose);

				ss.clear();
			}

		   private:
			inline Logger() {
			}

			inline Logger(Logger&& c) {
				this->channel_name = c.channel_name;
			}

// remove if 0 if this should be used
// keeping because it's useful
#if 0
		static std::string TimestampString() {
			std::chrono::time_point<std::chrono::system_clock> p = std::chrono::system_clock::now();

			std::time_t t = std::chrono::system_clock::to_time_t(p);
			std::string ts = std::string(std::ctime(&t));

			ts.back() = ']';
			ts.push_back(' ');
			ts.insert(ts.begin(), '[');

			return ts;
		}

#endif

			/**
			 * The channel name that the logger will use.
			 */
			std::string channel_name;
		};

		/**
		 * This macro can be used to create a logger statement marked
		 * with the source file and line it was emitted from.
		 */
#define LOGGER_MARKED " (", __FILE__, " line ", __LINE__, ")"

	} // namespace core
} // namespace xb2at