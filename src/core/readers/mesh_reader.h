#pragma once
#include <core.h>
#include <structs/mesh.h>
#include "base_reader.h"

namespace xb2at {
namespace core {
	
	enum class meshReaderStatus {
		Success,
		ErrorReadingHeader,
		ErrorReadingVertexData,
		ErrorReadingFaceData,
		ErrorReadingWeightData,
		ErrorReadingMorphData
	};

	inline std::string meshReaderStatusToString(meshReaderStatus status) {
		const char* status_str[] = {
			"Success",
			"Error reading mesh header",
			"Error reading vertex data",
			"Error reading face data",
			"Error reading weight data",
			"Error reading morph data",
		};

		return status_str[(int)status];
	}

	/**
	 * Options for meshReader::Read()
	 */
	struct meshReaderOptions {

		meshReaderOptions(std::vector<char>& fileData) 
			: file(fileData) {
			
		}

		/**
		 * Decompressed file data from XBC1.
		 */
		std::vector<char>& file;

		/**
		 * The result of the read operation.
		 */
		meshReaderStatus Result;
	};

	/**
	 * Reads meshes.
	 */
	struct meshReader : public base_reader {

		/**
		 * Read a mesh and output a deserialized structure.
		 *
		 * \param[in] opts Options.
		 */
		mesh::mesh Read(meshReaderOptions& opts);
		

	};


}
}