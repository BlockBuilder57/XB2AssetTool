#pragma once
#include <core.h>

namespace xb2at {
namespace core {

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

		template<typename T, typename ...Args>
		inline void info(const T value, Args... args) {
			if(!Logger::OutputFunction)
				return;

			std::ostringstream ss;

			//ss << TimestampString() << "[" << channel_name << "] " << Stringify(value, args...);
			ss << "[" << channel_name << "] " << Stringify(value, args...);
			
			Logger::OutputFunction(ss.str(), LogSeverity::Info);
			
			ss.clear();
		}		
		
		template<typename T, typename ...Args>
		inline void warn(const T value, Args... args) {
			if(!Logger::OutputFunction)
				return;

			std::ostringstream ss;
			ss << "[" << channel_name << "] " << Stringify(value, args...);
			
			Logger::OutputFunction(ss.str(), LogSeverity::Warning);
			
			ss.clear();
		}	

		template<typename T, typename ...Args>
		inline void error(const T value, Args... args) {
			if(!Logger::OutputFunction)
				return;

			std::ostringstream ss;
			ss << "[" << channel_name << "] " << Stringify(value, args...);
			
			Logger::OutputFunction(ss.str(), LogSeverity::Error);
			
			ss.clear();
		}
		
		template<typename T, typename ...Args>
		inline void verbose(const T value, Args... args) {
			if(!Logger::AllowVerbose)
				return;
			
			if(!Logger::OutputFunction)
				return;

			std::ostringstream ss;
			ss << "[" << channel_name << "] " << Stringify(value, args...);
			
			//if(Logger::OutputFunction)
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

		template<typename Head, typename... Tail>
		static std::string Stringify(Head h, Tail... t) {
			std::string s;
			StringifyImpl(s, h, t...);
			return s;
		}

		template<typename Head, typename...Tail>
		static void StringifyImpl(std::string& string, Head h, Tail... t) {
			StringifyImpl(string, h);
			StringifyImpl(string, t...);
		}

		template<typename T>
		static void StringifyImpl(std::string& string, T item) {
			std::ostringstream ss;
			ss << item;
			const std::string str = ss.str();

			string += str;
		}

		/**
		 * The channel name that the logger will use.
		 */
		std::string channel_name;
	};

	// use this macro to construct a logger statement
	// that has file and line marked. Useful for if you want to trace stuff.
#define LOGGER_MARKED " (" , __FILE__, " line " , __LINE__, ")"

}
}