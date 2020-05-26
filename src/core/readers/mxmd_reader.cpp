#include "mxmd_reader.h"
#include "streamhelper.h"
#include "endian_swap.h"

namespace xb2at {
	namespace core {

		mxmd::mxmd mxmdReader::Read(const mxmdReaderOptions& opts) {
			StreamHelper reader(stream);
			mxmd::mxmd data;

			// Read the initial header
			if (!reader.ReadType<mxmd::mxmd_header>(data)) {
				CheckedProgressUpdate("MXMD header could not be read", ProgressType::Error);
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

					for(int i = 0; i < data.model.morphControllers.count; ++i) {
						auto& mp = data.model.morphControllers.controls[i];
						reader.ReadType<mxmd::morph_control_info>(mp);
						
						if(mp.nameOffset1 != 0) {
							auto oldpos = stream.tellg();

							stream.seekg((data.modelStructOffset + data.model.morphControllersOffset + mp.nameOffset1), std::istream::beg);
							mp.name = reader.ReadString();

							stream.seekg(oldpos, std::istream::beg);
						}
					}

				}
			}

			CheckedProgressUpdate("MXMD reading successful", ProgressType::Info);
			return data;
		}

	}
}
