#include <core.h>
#include "structs/xbc1.h"
#include "base_reader.h"

namespace xb2at {
namespace core {

	enum class xbc1ReaderStatus {
		Success,
		ErrorReadingHeader,
		NotXBC1,
		ZlibError
	};

	inline std::string xbc1ReaderStatusToString(xbc1ReaderStatus status) {
		static const char* status_str[] = {
			"Success",
			"Error reading XBC1 header",
			"File is not XBC1",
			"Error decompressing Zlib data"
		};

		return status_str[(int)status];
	}

	/**
	 * Options to pass to xbc1Reader::Read().
	 */
	struct xbc1ReaderOptions {
		
		/**
		 * Offset where XBC1 file starts.
		 */
		int32 offset;

		/**
		 * Output directory.
		 */
		fs::path output_dir; 

		/**
		 * Whether or not we should save files in the output directory.
		 */
		bool save;

		xbc1ReaderStatus Result;
	};

	/**
	 * Reads and decompresses XBC1 files.
	 */
	struct xbc1Reader : public base_reader  {

		xbc1Reader(std::istream& input_stream)
			: stream(input_stream) {
		
		}

		/**
		 * Read and decompress a singular XBC1 file.
		 *
		 * \param[in] opts Options to pass to the reader.
		 */
		xbc1::xbc1 Read(xbc1ReaderOptions& opts);
		

	private:
		std::istream& stream;

		Logger logger = Logger::GetLogger("XBC1Reader");
	};

}
}