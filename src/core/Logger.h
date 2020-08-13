#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	struct Logger {

		// get a logger channel
		static Logger GetLogger(std::string group_name) {
			Logger l;
			l.group_name = group_name;
			return l;
		}

		// Logging functions

		// static stuff
		static std::function<void(const std::string, LogSeverity)> OutputFunction;
		static bool AllowVerbose;


		template<typename T, typename ...Args>
		inline void info(const T value, Args... args) {
			std::ostringstream ss;

			//ss << TimestampString() << "[" << group_name << "] " << Stringify(value, args...);
			ss << "[" << group_name << "] " << Stringify(value, args...);
			
			//if(Logger::OutputFunction)
			Logger::OutputFunction(ss.str(), LogSeverity::Info);
			
			ss.clear();
		}		
		
		template<typename T, typename ...Args>
		inline void warn(const T value, Args... args) {
			std::ostringstream ss;
			ss << "[" << group_name << "] " << Stringify(value, args...);
			
			//if(Logger::OutputFunction)
			Logger::OutputFunction(ss.str(), LogSeverity::Warning);
			
			ss.clear();
		}	

		template<typename T, typename ...Args>
		inline void error(const T value, Args... args) {
			std::ostringstream ss;
			ss << "[" << group_name << "] " << Stringify(value, args...);
			
			//if(Logger::OutputFunction)
			Logger::OutputFunction(ss.str(), LogSeverity::Error);
			
			ss.clear();
		}
		
		template<typename T, typename ...Args>
		inline void verbose(const T value, Args... args) {
			if(!Logger::AllowVerbose)
				return;
			
			std::ostringstream ss;
			ss << "[" << group_name << "] " << Stringify(value, args...);
			
			//if(Logger::OutputFunction)
			Logger::OutputFunction(ss.str(), LogSeverity::Verbose);
			
			ss.clear();
		}

	private:

		inline Logger() {
		}

		inline Logger(Logger&& c) {
			this->group_name = c.group_name;
		}

		static std::string TimestampString() {
			std::chrono::time_point<std::chrono::system_clock> p = std::chrono::system_clock::now();

			std::time_t t = std::chrono::system_clock::to_time_t(p);
			std::string ts = std::string(std::ctime(&t));

			ts.back() = ']';
			ts.push_back(' ');
			ts.insert(ts.begin(), '[');

			return ts;
		}

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

		std::string group_name;
	};

}
}