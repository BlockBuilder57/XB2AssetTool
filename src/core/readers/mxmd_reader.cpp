#include "mxmd_reader.h"
#include "streamhelper.h"
#include "endian_swap.h"

namespace xb2at {
	namespace core {

		mxmd::mxmd mxmdReader::Read(const mxmdReaderOptions& opts) {
			StreamHelper reader(stream);
			mxmd::mxmd data{};

			// Read the initial header
			if(!reader.ReadType<mxmd::mxmd_header>(data)) {
				CheckedProgressUpdate("MXMD header could not be read", ProgressType::Error);
				return data;
			}

			if(!strncmp(data.magic, "DMXM", sizeof("DMXM"))) {
				CheckedProgressUpdate("file is not MXMD", ProgressType::Error);
				return data;
			}

			if(data.modelStructOffset != 0) {
				stream.seekg(data.modelStructOffset, std::istream::beg);
				reader.ReadType<mxmd::model_info>(data.model);

				if(data.model.morphControllersOffset != 0) {
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

				if(data.model.morphNamesOffset != 0) {
					stream.seekg((data.modelStructOffset + data.model.morphNamesOffset), std::istream::beg);
					reader.ReadType<mxmd::morph_names_info>(data.model.morphNames);
					data.model.morphNames.morphNames.resize(data.model.morphNames.count);
					auto nextPos = stream.tellg();

					for(int i = 0; i < data.model.morphNames.count; ++i) {
						stream.seekg(nextPos, std::istream::beg);
						nextPos += sizeof(mxmd::morph_name_info);

						reader.ReadType<mxmd::morph_name_info>(data.model.morphNames.morphNames[i]);

						stream.seekg(data.modelStructOffset + data.model.morphNamesOffset + data.model.morphNames.morphNames[i].NameOffset);
						data.model.morphNames.morphNames[i].name = reader.ReadString();
					}
				}

				if(data.model.meshesOffset != 0) {
					data.model.meshes.resize(data.model.meshesCount);
					stream.seekg(data.modelStructOffset + data.model.meshesOffset, std::istream::beg);

					for(int i = 0; i < data.model.meshesCount; ++i) {
						reader.ReadType<mxmd::meshes_info>(data.model.meshes[i]);

						data.model.meshes[i].bbStart = reader.ReadVec3();
						data.model.meshes[i].bbEnd = reader.ReadVec3();
						reader.ReadType(data.model.meshes[i].radius);

						stream.seekg(data.modelStructOffset + data.model.meshes[i].tableOffset, std::istream::beg);
						data.model.meshes[i].descriptors.resize(data.model.meshes[i].tableCount);

						for(int j = 0; j < data.model.meshes[i].tableCount; ++j)
							reader.ReadType(data.model.meshes[i].descriptors[j]);
					}
				}

				if(data.model.nodesOffset != 0) {
					stream.seekg(data.modelStructOffset + data.model.nodesOffset, std::istream::beg);
					reader.ReadType<mxmd::skeleton_info>(data.model.skeleton);
					data.model.skeleton.nodes.resize(data.model.skeleton.boneCount);

					auto NextPos = data.modelStructOffset + data.model.nodesOffset + data.model.skeleton.nodeIdsOffset;
					for(int i = 0; i < data.model.skeleton.boneCount; ++i) {
						stream.seekg(NextPos, std::istream::beg);
						NextPos += sizeof(mxmd::node_info);

						reader.ReadType<mxmd::node_info>(data.model.skeleton.nodes[i]);

						stream.seekg(data.modelStructOffset + data.model.nodesOffset + data.model.skeleton.nodes[i].nameOffset, std::istream::beg);
						data.model.skeleton.nodes[i].name = reader.ReadString();
					}

					NextPos = data.modelStructOffset + data.model.nodesOffset + data.model.skeleton.nodeTmsOffset;
					for(int i = 0; i < data.model.skeleton.boneCount; ++i) {
						stream.seekg(NextPos, std::iostream::beg);

						// Quaternions are sizeof(float) = 4 (4 bytes == 32bits) * 4
						// You could do sizeof(float) * 4 but this is more clear
						NextPos += sizeof(quaternion);

						data.model.skeleton.nodes[i].scale = reader.ReadQuaternion();
						data.model.skeleton.nodes[i].rotation = reader.ReadQuaternion();
						data.model.skeleton.nodes[i].position = reader.ReadQuaternion();

						data.model.skeleton.nodes[i].parentTransform = reader.ReadQuaternion();
					}
				}
			}

			if(data.materialsOffset != 0) {
				stream.seekg(data.materialsOffset, std::iostream::beg);
				reader.ReadType<mxmd::materials_info>(data.materials);
				data.materials.mats.resize(data.materials.count);

				for(int i = 0; i < data.materials.count; ++i) {
					stream.seekg(data.materialsOffset + data.materials.offset + (i * sizeof(mxmd::material_info)));
					reader.ReadType<mxmd::material_info>(data.materials.mats[i]);

					stream.seekg(data.materialsOffset + data.materials.mats[i].nameOffset);
					data.materials.mats[i].name = reader.ReadString();
				}
			}

			CheckedProgressUpdate("MXMD reading successful", ProgressType::Info);
			return data;
		}

	}
}
