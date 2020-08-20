#pragma once
#include <core.h>
#include <structs/lbim.h>
#include "base_reader.h"

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

			miblReaderOptions(std::vector<char>& lbimFileData, std::vector<char>* fileData)
				: lbimFile(lbimFileData) {
				file = fileData;
			}

			/**
			 * Decompressed file data from XBC1 containing every MIBL for the MSRD textures.
			 * Is always index 1 of the MSRD files.
			 */
			std::vector<char>& lbimFile;

			/**
			 * Decompressed texture data from XBC1.
			 */
			std::vector<char>* file;

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
			 * Will be miblReaderStatus::Success if the read was successful.
			 */
			miblReaderStatus Result;
		};

		/**
		 * Reads MIBL textures.
		 */
		struct miblReader : public base_reader {

			/**
			 * Read a MIBL and output the texture data.
			 * This function does not decompress/unswizzle any data.
			 *
			 * \param[in] opts Reference to options to pass to the reader.
			 */
			mibl::texture Read(miblReaderOptions& opts);

		private:

			Logger logger = Logger::CreateChannel("LBIMReader");
		};


	}
}