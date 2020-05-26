/**
 * \file 
 * MXMD structures.
 */
#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	/**
	 * MXMD structures.
	 */
namespace mxmd {

	struct material_info {
		int32 nameOffset;

		byte unknown1[0x70];
	};

	struct material : public material_info {
		std::string name;
	};

	struct materials_info {
		int32 offset;
		int32 count;
	};

	struct materials : public materials_info {
		std::vector<material> mats;
	};

	struct node_info {
		int32 nameOffset;
		float unknown1;
		int32 unknown2;

		int32 id;
		int32 unknown3;
		int32 unknown4;

		quaternion scale;
		quaternion rotation;
		quaternion position;
		quaternion parentTransform;
	};

	struct node : public node_info {
		std::string name;
	};

	struct skeleton_info {
		int32 boneCount;
		int32 boneCount2;

		int32 nodeIdsOffset;
		int32 nodeTmsOffset;
	};

	struct skeleton : public skeleton_info {
		std::vector<node> nodes;
	};

	struct mesh_descriptor {
		int32 id;
		int32 descriptor;

		int16 vertTableIndex;
		int16 faceTableIndex;

		int16 unknown1;
		int16 materialID;

		byte unknown2[0xC];
		int16 unknown3;
		int16 lod;
		
		int32 unknown4;
		byte unknown5[0xC];
	};

	struct meshes_info {
		int32 tableOffset;
		int32 tableCount;
		int32 unknown1;

		vector3 bbStart;
		vector3 bbEnd;
		float radius;
	};

	struct meshes : public meshes_info {
		std::vector<mesh_descriptor> descriptors;
	};

	struct morph_name_info {
		int32 NameOffset;
		int32 unknown1;
		int32 unknown2;
		int32 unknown3;
	};

	struct morph_name : public morph_name_info {
		std::string name;
	};

	struct morph_names_info {
		int32 tableOffset;
		int32 count;

		byte unknown1[0x20];
	};

	struct morph_names : public morph_names_info {
		std::vector<morph_name> morphNames;
	};

	struct morph_control_info {
		int32 nameOffset1;
		int32 nameOffset2;

		byte unknown1[0x14];
	};

	struct morph_control : public morph_control_info {
		std::string name;
	};

	struct morph_controllers_info {
		int32 tableOffset;
		int32 count;

		byte unknown1[0x10];
	};

	struct morph_controllers : public morph_controllers_info {
		std::vector<morph_control> controls;
	};

	/**
	 * Model info
	 */
	struct model_info {
		int32 unknown1;
		vector3 bbStart;
		vector3 bbEnd;
		int32 meshesOffset;
		int32 meshesCount;
		int32 unknown2;
		int32 nodesOffset;

		byte unknown3[0x54];

		int32 morphControllersOffset;
		int32 morphNamesOffset;

	};

	struct model : public model_info {
		morph_controllers morphControllers;
		morph_names morphNames;
	};

	/**
	 * MXMD header
	 */
	struct mxmd_header {

		/**
		 * Magic value. Should be "DMXM"
		 */
		char magic[4];

		int32 version;

		int32 modelStructOffset;
		int32 materialsOffset;

		int32 unknown1;

		int32 vertexBufferOffset;
		int32 shadersOffset;
		int32 cachedTexturesTableOffset;
		int32 unknown2;
		int32 uncahcedTexturesTableOffset;

		byte unknown3[0x28];
	};

	/**
	 * MXMD data
	 */
	struct mxmd : public mxmd_header {
		model model;
		materials materials;
	};
}

}
}