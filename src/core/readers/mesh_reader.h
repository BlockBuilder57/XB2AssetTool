#pragma once
#include <core.h>
#include <structs/mesh.h>
#include "base_reader.h"

namespace xb2at {
namespace core {

	/**
	 * Options for meshReader::Read()
	 */
	struct meshReaderOptions {
		/**
		 * Decompressed file data from XBC1.
		 */
		std::vector<char>& file;
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
		mesh::mesh Read(const meshReaderOptions& opts);
		

	};


}
}