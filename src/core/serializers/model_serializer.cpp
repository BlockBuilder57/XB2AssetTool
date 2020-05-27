#include <serializers/model_serializer.h>
#include <fx/gltf.h>
#include "version.h"

namespace gltf = fx::gltf;
using vec3 = xb2at::core::vector3;

namespace xb2at {
namespace core {

	template <typename T>
	uint32 FlattenSerialize(std::vector<T> const& from, std::vector<uint8_t>& to, uint64& offset) {
		uint32 bytesToSerialize = sizeof(T) * (uint32)from.size();

		to.resize(to.size() + bytesToSerialize);
		std::memcpy(&to[offset], &from[0], bytesToSerialize);

		offset += bytesToSerialize;
		return bytesToSerialize;
	}

	void modelSerializer::Serialize(std::vector<mesh::mesh>& meshesToDump, mxmd::mxmd& mxmdData, const modelSerializerOptions& options) {
		fs::path outPath(options.outputDir);
		
		outPath = outPath / options.filename;

		if(options.OutputFormat == modelSerializerOptions::Format::GLTFBinary) {
			outPath.replace_extension(".glb");
		} else if (options.OutputFormat == modelSerializerOptions::Format::GLTFText) {
			outPath.replace_extension(".gltf");
		} else if (options.OutputFormat == modelSerializerOptions::Format::Dump) {
			outPath.replace_extension(".txt");
		}
		
		std::ofstream ofs(outPath.string(), (options.OutputFormat == modelSerializerOptions::Format::GLTFBinary) ? std::ofstream::binary : std::ostream::out);

		if (options.OutputFormat == modelSerializerOptions::Format::GLTFBinary || options.OutputFormat == modelSerializerOptions::Format::GLTFText) {
			// do gltf things

			gltf::Document doc{};
			doc.asset.generator = "XB2AssetTool " + std::string(version::tag);
			doc.asset.version = "2.0"; // glTF version, not generator version!

			for (mxmd::material mxmdMat : mxmdData.Materials.Materials) {
				gltf::Material material;
				material.name = mxmdMat.name;
				doc.materials.push_back(material);
			}

			for (int i = 0; i < mxmdData.Model.meshesCount; ++i) {
				gltf::Scene scene{};

				mesh::mesh& meshToDump = meshesToDump[i];

				mesh::vertex_table weightTbl = meshToDump.vertexTables.back();

				for (int j = 0; j < mxmdData.Model.Meshes[i].tableCount; ++j) {
					mxmd::mesh_descriptor desc = mxmdData.Model.Meshes[i].descriptors[j];

					mesh::vertex_table& vertTbl = meshToDump.vertexTables[desc.vertTableIndex];
					mesh::face_table& faceTbl = meshToDump.faceTables[desc.faceTableIndex];

					auto it = Where(meshToDump.morphData.morphDescriptors, [&](mesh::morph_descriptor& meshMorphDesc) {
							return meshMorphDesc.bufferId == desc.vertTableIndex;
					});

					if(it == meshToDump.morphData.morphDescriptors.end()) {
						// I'll care to write a better error message when it's not 5 AM.
						CheckedProgressUpdate("Error on mesh " + std::to_string(i), ProgressType::Error);
						continue;
					}

					mesh::morph_descriptor& morphDesc = *it;


					std::vector<uint16_t> indices(faceTbl.vertCount);
					for (int k = 0; k < faceTbl.vertCount; ++k)
						indices[k] = faceTbl.vertices[k];

					std::vector<vec3> positions = vertTbl.vertices;

					std::vector<vec3> normals(vertTbl.dataCount);
					for (int k = 0; k < vertTbl.dataCount; ++k)
						memcpy(&normals[k], &vertTbl.normals[k], sizeof(vec3));

					std::vector<color> vertexColors = vertTbl.vertexColor;

					std::vector<vector2> uv0 = vertTbl.uvPos[0];
					std::vector<vector2> uv1 = vertTbl.uvPos[1];
					std::vector<vector2> uv2 = vertTbl.uvPos[2];
					std::vector<vector2> uv3 = vertTbl.uvPos[3];

					// toss all vectors into a buffer
					// buffer format: indices, positions, normals, vertexColors, uv0, uv1, uv2, uv3
					gltf::Buffer buffer{};
					uint64 bufferTally = 0;

					uint32 indexSize = FlattenSerialize(indices, buffer.data, bufferTally);
					uint32 positionSize = FlattenSerialize(positions, buffer.data, bufferTally);
					uint32 normalSize = FlattenSerialize(normals, buffer.data, bufferTally);
					uint32 vertexColorsSize = FlattenSerialize(vertexColors, buffer.data, bufferTally);
					uint32 uv0Size = FlattenSerialize(uv0, buffer.data, bufferTally);
					uint32 uv1Size = FlattenSerialize(uv1, buffer.data, bufferTally);
					uint32 uv2Size = FlattenSerialize(uv2, buffer.data, bufferTally);
					uint32 uv3Size = FlattenSerialize(uv3, buffer.data, bufferTally);

					buffer.byteLength = (uint32)buffer.data.size();
					buffer.SetEmbeddedResource();

					// insert into document
					doc.buffers.push_back(buffer);


					// create bufferviews off of the buffers
					gltf::BufferView indexBufferView{};
					gltf::BufferView positionBufferView{};
					gltf::BufferView normalBufferView{};
					gltf::BufferView vertexColorsBufferView{};
					gltf::BufferView uv0BufferView{};
					gltf::BufferView uv1BufferView{};
					gltf::BufferView uv2BufferView{};
					gltf::BufferView uv3BufferView{};
					indexBufferView.buffer = (int32)doc.buffers.size() - 1;
					indexBufferView.byteLength = (int32)indexSize;
					indexBufferView.byteOffset = 0;
					positionBufferView.buffer = (int32)doc.buffers.size() - 1;
					positionBufferView.byteLength = (int32)positionSize;
					positionBufferView.byteOffset = (int32)indexSize;
					normalBufferView.buffer = (int32)doc.buffers.size() - 1;
					normalBufferView.byteLength = (int32)normalSize;
					normalBufferView.byteOffset = (int32)indexSize + positionSize;
					vertexColorsBufferView.buffer = (int32)doc.buffers.size() - 1;
					vertexColorsBufferView.byteLength = (int32)vertexColorsSize;
					vertexColorsBufferView.byteOffset = (int32)indexSize + positionSize + normalSize;
					uv0BufferView.buffer = (int32)doc.buffers.size() - 1;
					uv0BufferView.byteLength = (int32)uv0Size;
					uv0BufferView.byteOffset = (int32)indexSize + positionSize + normalSize + vertexColorsSize;
					uv1BufferView.buffer = (int32)doc.buffers.size() - 1;
					uv1BufferView.byteLength = (int32)uv1Size;
					uv1BufferView.byteOffset = (int32)indexSize + positionSize + normalSize + vertexColorsSize + uv0Size;
					uv2BufferView.buffer = (int32)doc.buffers.size() - 1;
					uv2BufferView.byteLength = (int32)uv2Size;
					uv2BufferView.byteOffset = (int32)indexSize + positionSize + normalSize + vertexColorsSize + uv0Size + uv1Size;
					uv3BufferView.buffer = (int32)doc.buffers.size() - 1;
					uv3BufferView.byteLength = (int32)uv3Size;
					uv3BufferView.byteOffset = (int32)indexSize + positionSize + normalSize + vertexColorsSize + uv0Size + uv1Size + uv2Size;

					doc.bufferViews.push_back(indexBufferView);
					doc.bufferViews.push_back(positionBufferView);
					doc.bufferViews.push_back(normalBufferView);
					doc.bufferViews.push_back(vertexColorsBufferView);
					doc.bufferViews.push_back(uv0BufferView);
					doc.bufferViews.push_back(uv1BufferView);
					doc.bufferViews.push_back(uv2BufferView);
					doc.bufferViews.push_back(uv3BufferView);


					// make accessors
					gltf::Accessor indexAccessor{};
					gltf::Accessor positionAccessor{};
					gltf::Accessor normalAccessor{};
					gltf::Accessor vertexColorsAccessor{};
					gltf::Accessor uv0Accessor{};
					gltf::Accessor uv1Accessor{};
					gltf::Accessor uv2Accessor{};
					gltf::Accessor uv3Accessor{};
					indexAccessor.bufferView = (int32)doc.accessors.size();
					indexAccessor.componentType = gltf::Accessor::ComponentType::UnsignedShort;
					indexAccessor.count = (uint32)indices.size();
					indexAccessor.type = gltf::Accessor::Type::Scalar;
					positionAccessor.bufferView = (int32)doc.accessors.size() + 1;
					positionAccessor.componentType = gltf::Accessor::ComponentType::Float;
					positionAccessor.count = (uint32)positions.size();
					positionAccessor.type = gltf::Accessor::Type::Vec3;
					normalAccessor.bufferView = (int32)doc.accessors.size() + 2;
					normalAccessor.componentType = gltf::Accessor::ComponentType::Float;
					normalAccessor.count = (uint32)normals.size();
					normalAccessor.type = gltf::Accessor::Type::Vec3;
					vertexColorsAccessor.bufferView = (int32)doc.accessors.size() + 3;
					vertexColorsAccessor.componentType = gltf::Accessor::ComponentType::UnsignedByte;
					vertexColorsAccessor.count = (uint32)vertexColors.size();
					vertexColorsAccessor.type = gltf::Accessor::Type::Vec4;
					uv0Accessor.bufferView = (int32)doc.accessors.size() + 4;
					uv0Accessor.componentType = gltf::Accessor::ComponentType::Float;
					uv0Accessor.count = (uint32)vertexColors.size();
					uv0Accessor.type = gltf::Accessor::Type::Vec2;
					uv1Accessor.bufferView = (int32)doc.accessors.size() + 5;
					uv1Accessor.componentType = gltf::Accessor::ComponentType::Float;
					uv1Accessor.count = (uint32)vertexColors.size();
					uv1Accessor.type = gltf::Accessor::Type::Vec2;
					uv2Accessor.bufferView = (int32)doc.accessors.size() + 6;
					uv2Accessor.componentType = gltf::Accessor::ComponentType::Float;
					uv2Accessor.count = (uint32)vertexColors.size();
					uv2Accessor.type = gltf::Accessor::Type::Vec2;
					uv3Accessor.bufferView = (int32)doc.accessors.size() + 7;
					uv3Accessor.componentType = gltf::Accessor::ComponentType::Float;
					uv3Accessor.count = (uint32)vertexColors.size();
					uv3Accessor.type = gltf::Accessor::Type::Vec2;

					// viewer requires this
					indexAccessor.min = { (float)*std::min_element(indices.begin(), indices.end()) };
					indexAccessor.max = { (float)*std::max_element(indices.begin(), indices.end()) };
					//FIXME ADDME FIXME ADDME FIXME ADDME (eventually)
					//positionAccessor.min = { -0.40553078055381775f, -0.010357379913330078f, -0.43219080567359924f };
					//positionAccessor.max = { -0.24855375289916992f, 0.010233467444777489f, -0.30006110668182373f };
					//normalAccessor.min = { 0, 1, 0 };
					//normalAccessor.max = { 0, 1, 0 };

					doc.accessors.push_back(indexAccessor);
					doc.accessors.push_back(positionAccessor);
					doc.accessors.push_back(normalAccessor);
					doc.accessors.push_back(vertexColorsAccessor);
					doc.accessors.push_back(uv0Accessor);
					doc.accessors.push_back(uv1Accessor);
					doc.accessors.push_back(uv2Accessor);
					doc.accessors.push_back(uv3Accessor);


					gltf::Mesh gltfMesh{};
					gltf::Primitive gltfPrimitive{};
					gltfMesh.name = "mesh" + std::to_string(j);
					gltfPrimitive.material = desc.materialID;
					gltfPrimitive.indices = (int32)doc.accessors.size() - 8;  // accessor 0
					gltfPrimitive.attributes["POSITION"] = (int32)doc.accessors.size() - 7;  // accessor 1
					gltfPrimitive.attributes["NORMAL"] = (int32)doc.accessors.size() - 6;  // accessor 2
					gltfPrimitive.attributes["COLOR_0"] = (int32)doc.accessors.size() - 5;  // accessor 3
					if (vertTbl.uvLayerCount > 0) // there's probably a better way to do this
						gltfPrimitive.attributes["TEXCOORD_0"] = (int32)doc.accessors.size() - 4;  // accessor 4
					if (vertTbl.uvLayerCount > 1)
						gltfPrimitive.attributes["TEXCOORD_1"] = (int32)doc.accessors.size() - 3;  // accessor 5
					if (vertTbl.uvLayerCount > 2)
						gltfPrimitive.attributes["TEXCOORD_2"] = (int32)doc.accessors.size() - 2;  // accessor 6
					if (vertTbl.uvLayerCount > 3)
						gltfPrimitive.attributes["TEXCOORD_3"] = (int32)doc.accessors.size() - 1;  // accessor 7
					gltfMesh.primitives.push_back(gltfPrimitive);

					doc.meshes.push_back(gltfMesh);


					gltf::Node node{};
					node.mesh = (int32)doc.meshes.size() - 1;

					doc.nodes.push_back(node);
					scene.nodes.push_back((int32)doc.nodes.size() - 1);
				}

				doc.scenes.push_back(scene);
				doc.scene = i;

				gltf::Save(doc, ofs, outPath.filename().string(), options.OutputFormat == modelSerializerOptions::Format::GLTFBinary);
			}

		} else if (options.OutputFormat == modelSerializerOptions::Format::Dump) {
			// do other things
		}

		ofs.close();
	}

}
}