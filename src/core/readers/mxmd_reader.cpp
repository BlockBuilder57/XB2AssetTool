#include "mxmd_reader.h"
#include "streamhelper.h"
#include "endian_swap.h"

namespace xb2at {
	namespace core {

		mxmd::mxmd mxmdReader::Read(const mxmdReaderOptions& opts) {
			StreamHelper reader(stream);
			mxmd::mxmd data{};

			// Read the initial header
			if (!reader.ReadType<mxmd::mxmd_header>(data)) {
				CheckedProgressUpdate("file could not be read", ProgressType::Error);
				return data;
			}

			if (!strncmp(data.magic, "DMXM", sizeof("DMXM"))) {
				CheckedProgressUpdate("file is not MXMD", ProgressType::Error);
				return data;
			}

			if (data.modelStructOffset != 0) {
				stream.seekg(data.modelStructOffset, std::istream::beg);
				reader.ReadType<mxmd::model_info>(data.model);

				if (data.model.morphControllersOffset != 0) {
					stream.seekg(data.modelStructOffset + data.model.morphControllersOffset, std::istream::beg);
					reader.ReadType<mxmd::morph_controllers_info>(data.model.morphControllers);

					data.model.morphControllers.controls.resize(data.model.morphControllers.count);
					
				}
			}

			CheckedProgressUpdate("MXMD reading successful", ProgressType::Info);
			return data;
		}

	}
}
