#pragma once
#include <core.h>
#include <base_progress_reporter.h>

namespace xb2at {
namespace core {

	/**
	 * Basic reader class.
	 * Inherits from base_progress_reporter
	 */
	struct base_reader : public base_progress_reporter {};

}
}