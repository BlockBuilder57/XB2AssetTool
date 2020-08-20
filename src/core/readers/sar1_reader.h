#include <core.h>
#include "structs/sar1.h"
#include "base_reader.h"

namespace xb2at {
namespace core {

	enum class sar1ReaderStatus {
		Success,
		ErrorReadingSAR1Header,
		ErrorReadingBCHeader,
		NotSAR1,
		NotBC
	};

	inline std::string sar1ReaderStatusToString(sar1ReaderStatus status) {
		// avoiding magic const by using constexpr
		constexpr static const char* status_str[] = {
			"Success",
			"Error reading SAR1 header",
			"Error reading BC header",
			"File is not SAR1",
			"Offset is not BC"
		};

		return status_str[(int)status];
	}

	/**
	 * Options to pass to sar1Reader::Read().
	 */
	struct sar1ReaderOptions {

		/**
		 * Output directory.
		 */
		fs::path output_dir; 

		/**
		 * Whether or not we should save files in the output directory.
		 */
		bool save;

		sar1ReaderStatus Result;
	};

	/**
	 * Reads and decompresses SAR1 files.
	 */
	struct sar1Reader : public base_reader  {

		sar1Reader(std::istream& input_stream)
			: stream(input_stream) {
		
		}

		/**
		 * Read and decompress a singular SAR1 file.
		 *
		 * \param[in] opts Options to pass to the reader.
		 */
		sar1::sar1 Read(sar1ReaderOptions& opts);
		

	private:
		std::istream& stream;
	};

}
}