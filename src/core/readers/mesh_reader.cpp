#include "streamhelper.h"
#include "endian_swap.h"
#include "mesh_reader.h"

#include "ivstream.h"
#include <algorithm>


namespace xb2at {
namespace core {

	mesh::mesh meshReader::Read(const meshReaderOptions& opts) {
		ivstream stream(opts.file);
		StreamHelper reader(stream);
		mesh::mesh mesh;

		// these could be imported into streamhelper
		// but i'd rather not import a soft dependency
		// of glm into streamhelper
		auto readVec3 = [&]() -> mesh::vector3 {
			float x;
			float y;
			float z;
			
			reader.ReadType<float>(x);
			reader.ReadType<float>(y);
			reader.ReadType<float>(z);

			return {x, y, z};
		};

		auto readVec2 = [&]() -> mesh::vector2 {
			float x;
			float y;
			
			reader.ReadType<float>(x);
			reader.ReadType<float>(y);

			
			return {x, y};
		};

		auto readQuat = [&]() -> mesh::quaternion { 
			float x;
			float y;
			float z;
			float w;
			
			reader.ReadType<float>(x);
			reader.ReadType<float>(y);
			reader.ReadType<float>(z);
			reader.ReadType<float>(w);
			
			return {x, y, z, w};
		};

		auto readu32u8Quat = [&]() -> mesh::quaternion {
			uint32 total;
			reader.ReadType<uint32>(total);

			float x = (float)(((sbyte*)&total)[0]) / 128.f;
			float y = (float)(((sbyte*)&total)[1]) / 128.f;
			float z = (float)(((sbyte*)&total)[2]) / 128.f;
			float w = (float)(((sbyte*)&total)[3]);

			return { x, y, z, w };
		};

		auto readu32s8Quat = [&]() -> mesh::quaternion { 
			uint32 total;
			reader.ReadType<uint32>(total);

			float x = (float) ( ((sbyte*)&total)[0] ) / 128.f;
			float y = (float) ( ((sbyte*)&total)[1] ) / 128.f;
			float z = (float) ( ((sbyte*)&total)[2] ) / 128.f;
			float w = (float) ( ((sbyte*)&total)[3] );

			return {x, y, z, w};
		};

		auto readu16Quat = [&]() -> mesh::quaternion { 
			uint64 total;
			reader.ReadType<uint64>(total);

			float x = (float) ( ((uint16*)&total)[0] ) / 65535.f;
			float y = (float) ( ((uint16*)&total)[1] ) / 65535.f;
			float z = (float) ( ((uint16*)&total)[2] ) / 65535.f;
			float w = (float) ( ((uint16*)&total)[3] ) / 65535.f;
			
			return {x, y, z, w};
		};

		// Read the mesh header
		if(!reader.ReadType<mesh::mesh_header>(mesh)) {
			CheckedProgressUpdate("file could not be read", ProgressType::Error);
			return mesh;
		}
		
		// Read vertex tables and vertex descriptors
		if(mesh.vertexTableOffset != 0) {
			mesh.vertexTables.resize(mesh.vertexTableCount);

			for(int i = 0; i < mesh.vertexTableCount; ++i) {
				CheckedProgressUpdate("Reading vertextable " + std::to_string(i), ProgressType::Info);
				stream.seekg(mesh.vertexTableOffset + (i * sizeof(mesh::vertex_table_header)), std::istream::beg);
				if(!reader.ReadType<mesh::vertex_table_header>(mesh.vertexTables[i])) {
					CheckedProgressUpdate("could not read vertex table header", ProgressType::Error);
					return mesh;
				}

				stream.seekg(mesh.vertexTables[i].descriptorOffset, std::istream::beg);

				mesh.vertexTables[i].vertexDescriptors.resize(mesh.vertexTables[i].descriptorCount);

				for(int j = 0; j < mesh.vertexTables[i].descriptorCount; ++j) {
					if(!reader.ReadType<mesh::vertex_descriptor>(mesh.vertexTables[i].vertexDescriptors[j])) {
						CheckedProgressUpdate("could not read vertex descriptor", ProgressType::Error);
						return mesh;
					}
				}
			}
		}
		
		// Read face table and vertices
		if(mesh.faceTableOffset != 0) {
			mesh.faceTables.resize(mesh.faceTableCount);

			for(int i = 0; i < mesh.faceTableCount; ++i) {
				CheckedProgressUpdate("Reading facetable " + std::to_string(i), ProgressType::Info);

				stream.seekg(mesh.faceTableOffset + (i * sizeof(mesh::face_table_header)), std::istream::beg);

				if(!reader.ReadType<mesh::face_table_header>(mesh.faceTables[i])) {
					CheckedProgressUpdate("Error reading vertex table header", ProgressType::Error);
					return mesh;
				}

				stream.seekg(mesh.dataOffset + mesh.faceTables[i].offset, std::istream::beg);

				mesh.faceTables[i].vertices.resize(mesh.faceTables[i].vertCount);
				for(int j = 0; j < mesh.faceTables[i].vertCount; ++j) {
					if(!reader.ReadType<uint16>(mesh.faceTables[i].vertices[j])){
						CheckedProgressUpdate("Error reading facetable " + std::to_string(i), ProgressType::Error);
						return mesh;
					}
				}
			}
		}

		// Read weight data and weight managers
		if(mesh.weightDataOffset != 0) {
			stream.seekg(mesh.weightDataOffset, std::istream::beg);
			
			if(!reader.ReadType<mesh::weight_data_header>(mesh.weightData)){
				CheckedProgressUpdate("Error reading weight data header", ProgressType::Error);
				return mesh;
			}

			mesh.weightData.weightManagers.resize(mesh.weightData.managerCount);
			stream.seekg(mesh.weightData.managerOffset, std::istream::beg);

			for(int i = 0; i < mesh.weightData.managerCount; ++i) {
				if(!reader.ReadType<mesh::weight_manager>(mesh.weightData.weightManagers[i])) {
					CheckedProgressUpdate("Error reading weight manager", ProgressType::Error);
					return mesh;
				}
			}
		}
		
		// Read morph data if we have it at all
		if(mesh.morphDataOffset > 0) {
			stream.seekg(mesh.morphDataOffset, std::istream::beg);

			reader.ReadType<int32>(mesh.morphData.morphDescriptorCount);
			reader.ReadType<int32>(mesh.morphData.morphDescriptorOffset);
			reader.ReadType<int32>(mesh.morphData.morphTargetCount);
			reader.ReadType<int32>(mesh.morphData.morphTargetOffset);

			mesh.morphData.morphDescriptors.resize(mesh.morphData.morphDescriptorCount);
			stream.seekg(mesh.morphData.morphDescriptorOffset, std::istream::beg);

			for(int i = 0; i < mesh.morphData.morphDescriptorCount; ++i) {
				if(!reader.ReadType<mesh::morph_descriptor_header>(mesh.morphData.morphDescriptors[i])) {
					CheckedProgressUpdate("Error reading morph descriptor header", ProgressType::Error);
					return mesh;
				}
			}

			mesh.morphData.morphTargets.resize(mesh.morphData.morphTargetCount);
			stream.seekg(mesh.morphData.morphTargetOffset, std::istream::beg);

			for(int i = 0; i < mesh.morphData.morphTargetCount; ++i) {
				if(!reader.ReadType<mesh::morph_target_header>(mesh.morphData.morphTargets[i])) {
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


			stream.seekg(mesh.dataOffset +  mesh.vertexTables[i].dataOffset, std::istream::beg);

			mesh.vertexTables[i].vertices.resize(mesh.vertexTables[i].dataCount);
			mesh.vertexTables[i].weights.resize(mesh.vertexTables[i].dataCount);

			ResizeMultiDimVec(mesh.vertexTables[i].uvPos, mesh.vertexTables[i].dataCount, 4);
			mesh.vertexTables[i].vertexColor.resize(mesh.vertexTables[i].dataCount);
			mesh.vertexTables[i].normals.resize(mesh.vertexTables[i].dataCount);
			mesh.vertexTables[i].weightStrengths.resize(mesh.vertexTables[i].dataCount);
			ResizeMultiDimVec(mesh.vertexTables[i].weightIds, mesh.vertexTables[i].dataCount, 4);

			for(int j = 0; j < mesh.vertexTables[i].dataCount; j++) {
				for(mesh::vertex_descriptor& desc : mesh.vertexTables[i].vertexDescriptors) {
					switch(desc.type) {
					case mesh::vertex_descriptor_type::Position:
						 mesh.vertexTables[i].vertices[j] = readVec3();
						break;

					case mesh::vertex_descriptor_type::BoneID:
						mesh.vertexTables[i].weights[j] = reader.ReadType<int32>();
						break;

					case mesh::vertex_descriptor_type::UV1:
					case mesh::vertex_descriptor_type::UV2:
					case mesh::vertex_descriptor_type::UV3:
						mesh.vertexTables[i].uvPos[j][desc.type - 5] = readVec2();
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
						mesh.vertexTables[i].normals[j] = readu32s8Quat();
						break;

					case mesh::vertex_descriptor_type::Weight16:
						mesh.vertexTables[i].weightStrengths[j] = readu16Quat();
						break;

					case mesh::vertex_descriptor_type::BoneID2:
						reader.ReadType<byte>(mesh.vertexTables[i].weightIds[j][0]);
						reader.ReadType<byte>(mesh.vertexTables[i].weightIds[j][1]);
						reader.ReadType<byte>(mesh.vertexTables[i].weightIds[j][2]);
						reader.ReadType<byte>(mesh.vertexTables[i].weightIds[j][3]);
						break;

					default:
						break;
					}
					stream.seekg(desc.size, std::istream::cur);
				}
			}
		}

		for(int i = 0; i < mesh.morphData.morphDescriptorCount; ++i) {

			mesh.morphData.morphDescriptors[i].targetIds.resize(mesh.morphData.morphDescriptors[i].targetCounts);
			stream.seekg(mesh.morphData.morphDescriptors[i].targetIdOffsets, std::istream::beg);

			for(int j = 0; j < mesh.morphData.morphDescriptors[i].targetCounts; ++j)
				reader.ReadType<int16>(mesh.morphData.morphDescriptors[i].targetIds[j]);

			mesh::morph_descriptor& desc = mesh.morphData.morphDescriptors[i];

			stream.seekg(mesh.dataOffset + mesh.morphData.morphTargets[desc.targetIndex].bufferOffset, std::istream::beg);
			
			for(int j = 0; j < mesh.morphData.morphTargets[desc.targetIndex].vertCount; ++j) {
				//mesh.vertexTables[desc.bufferId].vertices.resize(mesh.morphData.morphTargets[desc.targetIndex].vertCount);
				//mesh.vertexTables[desc.bufferId].normals.resize(mesh.morphData.morphTargets[desc.targetIndex].vertCount);

				mesh.vertexTables[desc.bufferId].vertices[j] = readVec3();
				mesh.vertexTables[desc.bufferId].normals[j] = readu32u8Quat();
				stream.seekg(mesh.morphData.morphTargets[desc.targetIndex].blockSize - 15, std::istream::cur); //TODO investigate other stuff
			}

			for(int j = 1; j < desc.targetCounts; ++j) {
				mesh.morphData.morphTargets[desc.targetIndex+j].vertices.resize(mesh.morphData.morphTargets[desc.targetIndex].vertCount);
				mesh.morphData.morphTargets[desc.targetIndex+j].normals.resize(mesh.morphData.morphTargets[desc.targetIndex].vertCount);

				stream.seekg(mesh.dataOffset + mesh.morphData.morphTargets[desc.targetIndex+j].bufferOffset, std::istream::beg);
				for(int k = 0; k < mesh.morphData.morphTargets[desc.targetIndex+j].vertCount; ++k) {
					int32 dummy;
					
					mesh::vector3 vert = readVec3();
					reader.ReadType<int32>(dummy);
					mesh::quaternion norm = readu32u8Quat();

					reader.ReadType<int32>(dummy);
					reader.ReadType<int32>(dummy);

					int32 index;
					reader.ReadType<int32>(index);

					mesh.morphData.morphTargets[desc.targetIndex+j].vertices[index] = vert;
					mesh.morphData.morphTargets[desc.targetIndex+j].normals[index] = norm;
				}
			}
		}

		CheckedProgressUpdate("Mesh reading successful", ProgressType::Info);
		return mesh;
	}

}
}
