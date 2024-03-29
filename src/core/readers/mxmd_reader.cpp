#include <xb2at/readers/mxmd_reader.h>
#include <xb2at/streamhelper.h>

namespace xb2at {
	namespace core {

		mxmd::mxmd mxmdReader::Read(mxmdReaderOptions& opts) {
			mco::BinaryReader reader(stream);
			mxmd::mxmd data {};

			// Read the initial header
			if(!reader.ReadSingleType((mxmd::mxmd_header&)data)) {
				opts.Result = mxmdReaderStatus::ErrorReadingHeader;
				return data;
			}

			if(strncmp(data.magic, "DMXM", sizeof(data.magic)) != 0) {
				opts.Result = mxmdReaderStatus::NotMXMD;
				return data;
			}

			logger.verbose("MXMD version: ", data.version, " (0x", std::hex, data.version, ")");

			if(data.modelStructOffset != 0) {
				stream.seekg(data.modelStructOffset, std::istream::beg);
				reader.ReadSingleType((mxmd::model_info&)data.Model);

				if(data.Model.morphControllersOffset != 0) {
					stream.seekg(data.modelStructOffset + data.Model.morphControllersOffset, std::istream::beg);
					reader.ReadSingleType((mxmd::morph_controllers_info&)data.Model.morphControllers);

					data.Model.morphControllers.controls.resize(data.Model.morphControllers.count);

					for(int i = 0; i < data.Model.morphControllers.count; ++i) {
						auto& mp = data.Model.morphControllers.controls[i];
						reader.ReadSingleType((mxmd::morph_control_info&)mp);

						if(mp.nameOffset1 != 0) {
							auto oldpos = stream.tellg();

							stream.seekg((data.modelStructOffset + data.Model.morphControllersOffset + mp.nameOffset1), std::istream::beg);
							mp.name = reader.ReadString();

							stream.seekg(oldpos, std::istream::beg);
						}
					}
				}

				if(data.Model.morphNamesOffset != 0) {
					stream.seekg(data.modelStructOffset + data.Model.morphNamesOffset, std::istream::beg);
					reader.ReadSingleType<mxmd::morph_names_info>(data.Model.morphNames);
					data.Model.morphNames.morphNames.resize(data.Model.morphNames.count);
					auto nextPos = data.modelStructOffset + data.Model.morphNamesOffset + data.Model.morphNames.tableOffset;

					for(int i = 0; i < data.Model.morphNames.count; ++i) {
						stream.seekg(nextPos, std::istream::beg);
						nextPos += sizeof(mxmd::morph_name_info);

						reader.ReadSingleType((mxmd::morph_name_info&)data.Model.morphNames.morphNames[i]);

						stream.seekg(data.modelStructOffset + data.Model.morphNamesOffset + data.Model.morphNames.morphNames[i].NameOffset, std::istream::beg);
						data.Model.morphNames.morphNames[i].name = reader.ReadString();
					}
				}

				if(data.Model.meshesOffset != 0) {
					data.Model.Meshes.resize(data.Model.meshesCount);
					stream.seekg(data.modelStructOffset + data.Model.meshesOffset, std::istream::beg);

					for(int i = 0; i < data.Model.meshesCount; ++i) {
						reader.ReadSingleType((mxmd::meshes_info&)data.Model.Meshes[i]);

						data.Model.Meshes[i].bbStart = ReadVec3(reader);
						data.Model.Meshes[i].bbEnd = ReadVec3(reader);
						reader.ReadSingleType(data.Model.Meshes[i].radius);

						stream.seekg(data.modelStructOffset + data.Model.Meshes[i].tableOffset, std::istream::beg);
						data.Model.Meshes[i].descriptors.resize(data.Model.Meshes[i].tableCount);

						for(int j = 0; j < data.Model.Meshes[i].tableCount; ++j)
							reader.ReadSingleType(data.Model.Meshes[i].descriptors[j]);
					}
				}

				if(data.Model.nodesOffset != 0) {
					stream.seekg(data.modelStructOffset + data.Model.nodesOffset, std::istream::beg);
					reader.ReadSingleType((mxmd::skeleton_info&)data.Model.Skeleton);
					data.Model.Skeleton.nodes.resize(data.Model.Skeleton.boneCount);

					auto NextPos = data.modelStructOffset + data.Model.nodesOffset + data.Model.Skeleton.nodeIdsOffset;
					for(int i = 0; i < data.Model.Skeleton.boneCount; ++i) {
						stream.seekg(NextPos, std::istream::beg);
						NextPos += sizeof(mxmd::node_info);

						reader.ReadSingleType((mxmd::node_info&)data.Model.Skeleton.nodes[i]);

						stream.seekg(data.modelStructOffset + data.Model.nodesOffset + data.Model.Skeleton.nodes[i].nameOffset, std::istream::beg);
						data.Model.Skeleton.nodes[i].name = reader.ReadString();
					}

					NextPos = data.modelStructOffset + data.Model.nodesOffset + data.Model.Skeleton.nodeTmsOffset;
					for(int i = 0; i < data.Model.Skeleton.boneCount; ++i) {
						stream.seekg(NextPos, std::iostream::beg);

						// Quaternions are sizeof(float) = 4 (4 bytes == 32bits) * 4
						// You could do sizeof(float) * 4 but this is more clear
						NextPos += sizeof(quaternion);

						data.Model.Skeleton.nodes[i].scale = ReadQuaternion(reader);
						data.Model.Skeleton.nodes[i].rotation = ReadQuaternion(reader);
						data.Model.Skeleton.nodes[i].position = ReadQuaternion(reader);

						data.Model.Skeleton.nodes[i].parentTransform = ReadQuaternion(reader);
					}
				}
			}

			if(data.materialsOffset != 0) {
				stream.seekg(data.materialsOffset, std::iostream::beg);
				reader.ReadSingleType((mxmd::materials_info&)data.Materials);
				data.Materials.Materials.resize(data.Materials.count);

				for(int i = 0; i < data.Materials.count; ++i) {
					stream.seekg(data.materialsOffset + data.Materials.offset + (i * sizeof(mxmd::material_info)));
					reader.ReadSingleType((mxmd::material_info&)data.Materials.Materials[i]);

					stream.seekg(data.materialsOffset + data.Materials.Materials[i].nameOffset);
					data.Materials.Materials[i].name = reader.ReadString();
				}
			}

			opts.Result = mxmdReaderStatus::Success;
			return data;
		}

	} // namespace core
} // namespace xb2at
