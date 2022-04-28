//
// Created by block on 7/23/2021.
//

#ifndef XB2AT_ILOGGERSINK_H
#define XB2AT_ILOGGERSINK_H

#include <string>

namespace xb2at::core {

	struct LoggerMessage {
		enum class Severity {
			Verbose,
			Info,
			Warning,
			Error,
			AssertionFailure
		};

		Severity severity;

		std::string message;

		//std::optional<fmt::format_args> format_args;
	};

	/**
	 * A base logger sink.
	 */
	struct LoggerSink {
		virtual void ProcessMessage(LoggerMessage&&) = 0;
	};

	/**
	 * Set the global logger sink.
	 */
	void SetLoggerSink(ILoggerSink* logger);

	struct Logger {

		void Info();

		void Assert(bool expr) const;


	   private:

		/**
		 * Encapsulation helper to avoid stuff.
		 */
		static void LogInto(LoggerMessage&&);

	};

	/// fancy assert
#define XB2AT_ASSERT(logger, x) logger.Assert(!(x));
}

#endif //XB2AT_ILOGGERSINK_H
