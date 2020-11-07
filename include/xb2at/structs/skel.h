/**
 * \file 
 * SKEL structures.
 */
#pragma once
#include <xb2at/core.h>
#include <xb2at/structs/sar1.h>

namespace xb2at {
	namespace core {

		/**
		 * SKEL structures.
		 */
		namespace skel {

			enum Items {
				NodeParents = 2,
				Nodes,
				Transforms
			};

			struct transform {
				quaternion position;
				quaternion rotation;
				quaternion scale;
			};

			struct node_data {
				int32 offset;
				char unknown1[0xC]; // pretty sure this is just padding
			};

			struct node : public node_data {
				std::string name;
			};

			struct toc {
				int32 offset;
				int32 unknown1;
				int32 count;
				int32 unknown2;
			};

			/**
			 * SKEL header.
			 */
			struct header {
				/**
				 * Magic value. Should be "SKEL".
				 */
				char magic[4];

				int32 unknown1;
				int32 unknown2;
			};

			/**
			 * SKEL data.
			 */
			struct skel : public header {
				toc tocItems[9];
				std::vector<unsigned short> nodeParents;
				std::vector<node> nodes;
				std::vector<transform> transforms;
			};
		} // namespace skel

		static_assert(sizeof(skel::header) == 12, "[xb2at.skel.header] Invalid SKEL header size!");
		static_assert(sizeof(skel::toc) == 16, "[xb2at.skel.toc] Invalid SKEL TOC size!");
		static_assert(sizeof(skel::node_data) == 16, "[xb2at.skel.node_data] Invalid SKEL node_data size!");

	} // namespace core
} // namespace xb2at