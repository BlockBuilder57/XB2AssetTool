/**
 * \file 
 * Mesh structures.
 */
#pragma once
#include <xb2at/core/Stream.h>
#include <xb2at/core/StorageMathTypes.h>

namespace xb2at::core::mesh {

			enum vertex_descriptor_type : std::uint16_t {
				Position,
				Weight32,
				BoneID,
				WeightID,
				UV1 = 5,
				UV2,
				UV3,
				Unknown8 = 8,
				Unknown14 = 14,
				Normal32 = 15,
				Tangent16,
				VertexColor,
				Normal = 28,
				Tangent,
				Normal2 = 32,
				Reflection,
				Weight16 = 41,
				BoneID2,
				MorphNormal,
				MorphVertexID
			};

			struct face_table_header {
				std::uint32_t offset;
				std::uint32_t vertCount;

				std::uint8_t unknown[0xC];
			};

			struct face_table : public face_table_header {
				std::vector<std::uint16_t> vertices;
			};

			struct vertex_descriptor {
				vertex_descriptor_type type;
				std::int16_t size;
			};

			struct weight_manager {
				std::uint32_t unknown1;
				std::uint32_t offset;
				std::uint32_t count;

				char unknown2[0x11];
				std::uint8_t lod;
				char unknown3[0xA];
			};

			struct weight_data_header {
				std::uint32_t managerCount;
				std::uint32_t managerOffset;

				std::int16_t vertexIndex;
				std::int16_t unknown;

				std::uint32_t offset2;
			};

			struct weight_data : public weight_data_header {
				std::vector<weight_manager> weightManagers;
			};

			struct morph_data_header {
				std::uint32_t morphDescriptorCount;
				std::uint32_t morphDescriptorOffset;
				std::uint32_t morphTargetCount;
				std::uint32_t morphTargetOffset;
			};

			struct morph_descriptor_header {
				std::uint32_t bufferId;

				std::uint32_t targetIndex;
				std::uint32_t targetCounts;
				std::uint32_t targetIdOffsets;

				std::uint32_t unknown1;
			};

			struct morph_descriptor : public morph_descriptor_header {
				std::vector<std::int16_t> targetIds;
			};

			struct morph_target_header {
				std::uint32_t bufferOffset;
				std::uint32_t vertCount;
				std::uint32_t blockSize;

				std::int16_t unknown1;
				std::int16_t type;
			};

			struct morph_target : public morph_target_header {
				std::vector<vector3> vertices;
				std::vector<quaternion> normals;
			};

			struct morph_data : public morph_data_header {
				std::vector<morph_descriptor> morphDescriptors;
				std::vector<morph_target> morphTargets;
			};

			struct vertex_table_header {
				std::uint32_t dataOffset;
				std::uint32_t dataCount;
				std::uint32_t blockSize;

				std::uint32_t descriptorOffset;
				std::uint32_t descriptorCount;

				char unknown1[0xC];
			};

			struct vertex_table : public vertex_table_header {
				std::vector<vertex_descriptor> vertexDescriptors;

				// not in order but i'll fix it later
				std::vector<vector3> vertices;
				std::vector<std::uint32_t> weightTableIndex;
				std::vector<std::vector<vector2>> uvPos;
				std::uint32_t uvLayerCount;
				std::vector<Rgba32> vertexColor;
				std::vector<quaternion> normals;
				std::vector<quaternion> weightStrengths;

				// maybe TODO: a MultiDim<T> using for vector<vector<T>>?
				std::vector<std::vector<std::uint8_t>> weightIds;
			};

			struct mesh_header {
				std::uint32_t vertexTableOffset;
				std::uint32_t vertexTableCount;
				std::uint32_t faceTableOffset;
				std::uint32_t faceTableCount;

				char unknown1[0xC];

				std::uint32_t unkOffset1;
				std::uint32_t unkOffset2;
				std::uint32_t unkOffset2Count;

				std::uint32_t morphDataOffset;
				std::uint32_t dataSize;
				std::uint32_t dataOffset;
				std::uint32_t weightDataSize;
				std::uint32_t weightDataOffset;

				char unknown2[0x14];
			};

			struct mesh : public mesh_header {
				std::vector<vertex_table> vertexTables;
				std::vector<face_table> faceTables;

				weight_data weightData;
				morph_data morphData;
			};

		} // namespace xb2at
