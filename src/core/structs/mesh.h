/**
 * \file 
 * Mesh structures.
 */
#pragma once
#include <core.h>

namespace xb2at {
namespace core {

namespace mesh {

	struct vector2 {
		float x;
		float y;
	};

	struct vector3 {
		float x;
		float y;
		float z;
	};

	struct quaternion {
		float x;
		float y;
		float z;
		float w;
	};

	struct color {
		byte r;
		byte g;
		byte b;
		byte a;
	};

	enum vertex_descriptor_type : int16 {
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
		int32 offset;
		int32 vertCount;

		byte unknown[0xC];
	};

	struct face_table : public face_table_header {
		std::vector<uint16> vertices;
	};

	struct vertex_descriptor {
		int16 type;
		int16 size;
	};

	struct weight_manager {
		int32 unknown1;
		int32 offset;
		int32 count;
	
		char unknown2[0x11];
		byte lod;
		char unknown3[0xA];
	};

	struct weight_data_header {
		int32 managerCount;
		int32 managerOffset;

		int16 vertexIndex;
		int16 unknown;

		int32 offset2;
	};

	struct weight_data : public weight_data_header {
		std::vector<weight_manager> weightManagers;
	};

	struct morph_data_header {
		int32 morphDescriptorCount;		
		int32 morphDescriptorOffset;
	};

	struct morph_descriptor_header {
		int32 bufferId;

		int32 targetIndex;
		int32 targetCounts;
		int32 targetIdOffsets;

		int32 unknown1;
	};

	struct morph_descriptor : public morph_descriptor_header {
		std::vector<int16> targetIds;
	};

	struct morph_target_header {
		int32 bufferOffset;
		int32 vertCount;
		int32 blockSize;

		int16 unknown1;
		int16 type;
	};

	struct morph_target : public morph_target_header {
		std::vector<vector3> vertices;
		std::vector<quaternion> normals;
	};

	struct morph_data : public morph_data_header {
		std::vector<morph_descriptor> morphDescriptors;

		int32 morphTargetCount;
		int32 morphTargetOffset;

		std::vector<morph_target> morphTargets;
	};


	struct vertex_table_header {
		int32 dataOffset;
		int32 dataCount;
		int32 blockSize;

		int32 descriptorOffset;
		int32 descriptorCount;

		char unknown1[0xC];
	};

	struct vertex_table : public vertex_table_header {
		std::vector<vertex_descriptor> vertexDescriptors;

		// not in order but i'll fix it later
		std::vector<vector3> vertices;
		std::vector<int32> weights;
		std::vector<std::vector<vector2>> uvPos;
		int32 uvLayerCount;
		std::vector<color> vertexColor;
		std::vector<quaternion> normals;
		std::vector<quaternion> weightStrengths;
		std::vector<std::vector<byte>> weightIds;
	};

	struct mesh_header {
		int32 vertexTableOffset;
		int32 vertexTableCount;
		int32 faceTableOffset;
		int32 faceTableCount;

		char unknown1[0xC];

		int32 unkOffset1;
		int32 unkOffset2;
		int32 unkOffset2Count;

		int32 morphDataOffset;
		int32 dataSize;
		int32 dataOffset;
		int32 weightDataSize;
		int32 weightDataOffset;

		char unknown2[0x14];
	};

	struct mesh : public mesh_header {
		std::vector<vertex_table> vertexTables;
		std::vector<face_table> faceTables;
		
		weight_data weightData;
		morph_data morphData;
	};


}

}
}
