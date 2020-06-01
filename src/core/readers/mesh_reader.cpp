#include "streamhelper.h"
#include "endian_swap.h"
#include "mesh_reader.h"

#include "ivstream.h"
#include <algorithm>


namespace xb2at {
namespace core {

	mesh::mesh meshReader::Read(meshReaderOptions& opts) {
		ivstream stream(opts.file);
		StreamHelper reader(stream);
		mesh::mesh mesh;

		// Read the mesh header
		if(!reader.ReadType<mesh::mesh_header>(mesh)) {
			opts.Result = meshReaderStatus::ErrorReadingHeader;
			return mesh;
		}
		
		// Read vertex tables and vertex descriptors
		if(mesh.vertexTableOffset != 0) {
			mesh.vertexTables.resize(mesh.vertexTableCount);

			for(int i = 0; i < mesh.vertexTableCount; ++i) {
				PROGRESS_UPDATE(ProgressType::Verbose, "Reading mesh vertex table " << i)

				stream.seekg(mesh.vertexTableOffset + (i * sizeof(mesh::vertex_table_header)), std::istream::beg);
				if(!reader.ReadType<mesh::vertex_table_header>(mesh.vertexTables[i])) {
					opts.Result = meshReaderStatus::ErrorReadingVertexData;
					return mesh;
				}

				stream.seekg(mesh.vertexTables[i].descriptorOffset, std::istream::beg);

				mesh.vertexTables[i].vertexDescriptors.resize(mesh.vertexTables[i].descriptorCount);

				for(int j = 0; j < mesh.vertexTables[i].descriptorCount; ++j) {
					PROGRESS_UPDATE(ProgressType::Verbose, "Reading mesh vertex descriptor " << j)
					if(!reader.ReadType<mesh::vertex_descriptor>(mesh.vertexTables[i].vertexDescriptors[j])) {
						opts.Result = meshReaderStatus::ErrorReadingVertexData;
						return mesh;
					}
				}
			}
		}
		
		// Read face table and vertices
		if(mesh.faceTableOffset != 0) {
			mesh.faceTables.resize(mesh.faceTableCount);

			for(int i = 0; i < mesh.faceTableCount; ++i) {
				PROGRESS_UPDATE(ProgressType::Verbose, "Reading mesh face table " << i)

				stream.seekg(mesh.faceTableOffset + (i * sizeof(mesh::face_table_header)), std::istream::beg);

				if(!reader.ReadType<mesh::face_table_header>(mesh.faceTables[i])) {
					opts.Result = meshReaderStatus::ErrorReadingFaceData;
					return mesh;
				}

				stream.seekg(mesh.dataOffset + mesh.faceTables[i].offset, std::istream::beg);

				mesh.faceTables[i].vertices.resize(mesh.faceTables[i].vertCount);
				for(int j = 0; j < mesh.faceTables[i].vertCount; ++j) {
					if(!reader.ReadType<uint16>(mesh.faceTables[i].vertices[j])) {
						opts.Result = meshReaderStatus::ErrorReadingFaceData;
						//PROGRESS_UPDATE(ProgressType::Error, "Error reading face table " << i)
						return mesh;
					}
				}
			}
		}

		// Read weight data and weight managers
		if(mesh.weightDataOffset != 0) {
			PROGRESS_UPDATE(ProgressType::Verbose, "Reading mesh weight data")

			stream.seekg(mesh.weightDataOffset, std::istream::beg);
			
			if(!reader.ReadType<mesh::weight_data_header>(mesh.weightData)) {
				opts.Result = meshReaderStatus::ErrorReadingWeightData;
				return mesh;
			}

			mesh.weightData.weightManagers.resize(mesh.weightData.managerCount);
			stream.seekg(mesh.weightData.managerOffset, std::istream::beg);

			for(int i = 0; i < mesh.weightData.managerCount; ++i) {
				if(!reader.ReadType<mesh::weight_manager>(mesh.weightData.weightManagers[i])) {
					opts.Result = meshReaderStatus::ErrorReadingWeightData;
					CheckedProgressUpdate("Error reading weight manager", ProgressType::Error);
					return mesh;
				}
			}
		}
		
		// Read morph data if we have it at all
		if(mesh.morphDataOffset > 0) {
			PROGRESS_UPDATE(ProgressType::Verbose, "Reading mesh morph data")

			stream.seekg(mesh.morphDataOffset, std::istream::beg);

			if (!reader.ReadType<mesh::morph_data_header>(mesh.morphData)) {
				opts.Result = meshReaderStatus::ErrorReadingMorphData;
				return mesh;
			}

			mesh.morphData.morphDescriptors.resize(mesh.morphData.morphDescriptorCount);
			stream.seekg(mesh.morphData.morphDescriptorOffset, std::istream::beg);

			for(int i = 0; i < mesh.morphData.morphDescriptorCount; ++i) {
				if(!reader.ReadType<mesh::morph_descriptor_header>(mesh.morphData.morphDescriptors[i])) {
					opts.Result = meshReaderStatus::ErrorReadingMorphData;
					CheckedProgressUpdate("Error reading morph descriptor header", ProgressType::Error);
					return mesh;
				}
			}

			mesh.morphData.morphTargets.resize(mesh.morphData.morphTargetCount);
			stream.seekg(mesh.morphData.morphTargetOffset, std::istream::beg);

			for(int i = 0; i < mesh.morphData.morphTargetCount; ++i) {
				if(!reader.ReadType<mesh::morph_target_header>(mesh.morphData.morphTargets[i])) {
					opts.Result = meshReaderStatus::ErrorReadingMorphData;
					CheckedProgressUpdate("Error reading morph target header", ProgressType::Error);
					return mesh;
				}

				auto getMaxVertCount = [&]() -> int32 {
					// obtain a iterator to the element with the largest vertex count
					// (we do this by supplying a custom comparision function)
					auto it = std::max_element(mesh.morphData.morphTargets.begin(), mesh.morphData.morphTargets.end(), [](const mesh::morph_target& l, const mesh::morph_target& r) {
						// TODO fix
						return std::max(l.vertCount, r.vertCount) == r.vertCount;
					});

					// return the vertex count from the iterator we recieve
					return (*it).vertCount;
				};
				
				// resize
				mesh.morphData.morphTargets[i].vertices.resize(getMaxVertCount());
				mesh.morphData.morphTargets[i].normals.resize(getMaxVertCount());
			}

		}

		
		for(int i = 0; i < mesh.vertexTableCount; ++i) {
			PROGRESS_UPDATE(ProgressType::Verbose, "Reading mesh vertex data table for vertex table " << i)

			stream.seekg(mesh.dataOffset +  mesh.vertexTables[i].dataOffset, std::istream::beg);

			mesh.vertexTables[i].vertices.resize(mesh.vertexTables[i].dataCount);
			mesh.vertexTables[i].weights.resize(mesh.vertexTables[i].dataCount);

			ResizeMultiDimVec(mesh.vertexTables[i].uvPos, 4, mesh.vertexTables[i].dataCount);
			mesh.vertexTables[i].vertexColor.resize(mesh.vertexTables[i].dataCount);
			mesh.vertexTables[i].normals.resize(mesh.vertexTables[i].dataCount);
			mesh.vertexTables[i].weightStrengths.resize(mesh.vertexTables[i].dataCount);
			ResizeMultiDimVec(mesh.vertexTables[i].weightIds, 4, mesh.vertexTables[i].dataCount);

			for(int j = 0; j < mesh.vertexTables[i].dataCount; j++) {
				for(mesh::vertex_descriptor& desc : mesh.vertexTables[i].vertexDescriptors) {
					switch(desc.type) {
					case mesh::vertex_descriptor_type::Position:
						 mesh.vertexTables[i].vertices[j] = reader.ReadVec3();
						break;

					case mesh::vertex_descriptor_type::BoneID:
						mesh.vertexTables[i].weights[j] = reader.ReadType<int32>();
						break;

					case mesh::vertex_descriptor_type::UV1:
					case mesh::vertex_descriptor_type::UV2:
					case mesh::vertex_descriptor_type::UV3:
						mesh.vertexTables[i].uvPos[desc.type - 5][j] = reader.ReadVec2();
						if(desc.type - 4 > mesh.vertexTables[i].uvLayerCount)
							mesh.vertexTables[i].uvLayerCount = desc.type - 4;
						break;

					case mesh::vertex_descriptor_type::VertexColor:
						 reader.ReadType<byte>(mesh.vertexTables[i].vertexColor[j].a);
						 reader.ReadType<byte>(mesh.vertexTables[i].vertexColor[j].r);
						 reader.ReadType<byte>(mesh.vertexTables[i].vertexColor[j].g);
						 reader.ReadType<byte>(mesh.vertexTables[i].vertexColor[j].b);
						break;

					case mesh::vertex_descriptor_type::Normal:
					case mesh::vertex_descriptor_type::Normal2:
						mesh.vertexTables[i].normals[j] = reader.ReadS8Quaternion();
						break;

					case mesh::vertex_descriptor_type::Weight16:
						mesh.vertexTables[i].weightStrengths[j] = reader.ReadU16Quaternion();
						break;

					case mesh::vertex_descriptor_type::BoneID2:
						reader.ReadType<byte>(mesh.vertexTables[i].weightIds[0][j]);
						reader.ReadType<byte>(mesh.vertexTables[i].weightIds[1][j]);
						reader.ReadType<byte>(mesh.vertexTables[i].weightIds[2][j]);
						reader.ReadType<byte>(mesh.vertexTables[i].weightIds[3][j]);
						break;

					default:
						break;
					}
					stream.seekg(desc.size, std::istream::cur);
				}
			}
		}

		for(int i = 0; i < mesh.morphData.morphDescriptorCount; ++i) {

			mesh::morph_descriptor& desc = mesh.morphData.morphDescriptors[i];

			desc.targetIds.resize(desc.targetCounts);
			stream.seekg(desc.targetIdOffsets, std::istream::beg);

			for(int j = 0; j < mesh.morphData.morphDescriptors[i].targetCounts; ++j)
				reader.ReadType<int16>(desc.targetIds[j]);

			int morphTargetOffset = mesh.dataOffset + mesh.morphData.morphTargets[desc.targetIndex].bufferOffset;
			stream.seekg(morphTargetOffset, std::istream::beg);
			
			for(int j = 0; j < mesh.morphData.morphTargets[desc.targetIndex].vertCount; ++j) {
				stream.seekg(morphTargetOffset + (0x20 * j), std::istream::beg);

				mesh.vertexTables[desc.bufferId].vertices[j] = reader.ReadVec3();
				mesh.vertexTables[desc.bufferId].normals[j] = reader.ReadU8Quaternion();
			}

			// j = 2 as we skip the basis, then skip something else I don't know what it is god help me
			for(int j = 2; j < mesh.morphData.morphTargetCount; ++j) {
				mesh.morphData.morphTargets[desc.targetIndex+j].vertices.resize(mesh.morphData.morphTargets[desc.targetIndex].vertCount);
				mesh.morphData.morphTargets[desc.targetIndex+j].normals.resize(mesh.morphData.morphTargets[desc.targetIndex].vertCount);

				stream.seekg(mesh.dataOffset + mesh.morphData.morphTargets[desc.targetIndex+j].bufferOffset, std::istream::beg);
				for(int k = 0; k < mesh.morphData.morphTargets[desc.targetIndex+j].vertCount; ++k) {
					int32 dummy;
					
					vector3 vert = reader.ReadVec3();
					reader.ReadType<int32>(dummy);
					quaternion norm = reader.ReadS8Quaternion();

					reader.ReadType<int32>(dummy);
					reader.ReadType<int32>(dummy);

					int32 index;
					reader.ReadType<int32>(index);

					mesh.morphData.morphTargets[desc.targetIndex+j].vertices[index] = vert;
					mesh.morphData.morphTargets[desc.targetIndex+j].normals[index] = norm;
				}
			}
		}

		opts.Result = meshReaderStatus::Success;
		CheckedProgressUpdate("Mesh reading successful", ProgressType::Info);
		return mesh;
	}

}
}
