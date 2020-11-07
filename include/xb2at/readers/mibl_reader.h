#pragma once
#include <xb2at/core.h>
#include <modeco/Logger.h>

#include <xb2at/structs/mibl.h>
#include <xb2at/structs/xbc1.h>

namespace xb2at {
namespace core {

		enum class miblReaderStatus {
			Success,
			ErrorReadingHeader,
			NotMIBL
		};

		inline std::string miblReaderStatusToString(miblReaderStatus status) {
			// avoiding magic const by using constexpr
			constexpr static const char* status_str[] = {
				"Success",
				"Error reading the MIBL header",
				"File is not a MIBL file, or the version is invalid"
			};

			return status_str[(int)status];
		}

		/**
		 * Options to pass to the miblReader.
		 */
		struct miblReaderOptions {

			// TODO(lily): rename some of these members so they're not as confusing

			miblReaderOptions(std::vector<char>& miblFileData, xbc1::xbc1* fileData)
				: miblFile(miblFileData) {
				file = fileData;
			}

			/**
			 * Decompressed file data from XBC1 containing every MIBL for the MSRD textures.
			 * Is always index 1 of the MSRD files.
			 */
			std::vector<char>& miblFile;

			/**
			 * Decompressed texture data as an XBC1 file.
			 */
			xbc1::xbc1* file;

			/**
			 * Start offset of the texture.
			 */
			int32 offset;

			/**
			 * Size of the texture data.
			 */
			int32 size;

			/**
			 * The result of the read operation.
			 * Will be miblReaderStatus::Success if the read was successful, any other
			 * value to indicate a varying error state.
			 */
			miblReaderStatus Result;
		};

		/**
		 * Reads MIBL textures.
		 */
		struct miblReader {

			/**
			 * Read a MIBL and output the texture data.
			 * This function does not decompress/unswizzle any data.
			 *
			 * \param[in] opts Reference to options to pass to the reader.
			 */
			mibl::texture Read(miblReaderOptions& opts);

		private:

			mco::Logger logger = mco::Logger::CreateLogger("MIBLReader");
		};


	}
}