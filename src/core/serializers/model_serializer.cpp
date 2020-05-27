#include <serializers/model_serializer.h>
#include <fx/gltf.h>
#include "version.h"

namespace gltf = fx::gltf;
using vec3 = xb2at::core::vector3;

namespace xb2at {
namespace core {

	template <typename T>
	uint32 serialize(std::vector<T> const& from, std::vector<uint8_t>& to, std::size_t offset)
	{
		uint32 bytesToSerialize = sizeof(T) * (uint32)from.size();

		to.resize(to.size() + bytesToSerialize);
		std::memcpy(&to[offset], &from[0], bytesToSerialize);

		return bytesToSerialize;
	}

	void modelSerializer::Serialize(std::vector<mesh::mesh>& meshesToDump, const modelSerializerOptions& options) {
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

			for (mesh::mesh meshToDump : meshesToDump) {
				gltf::Document doc{};
				doc.asset.generator = "XB2AssetTool " + std::string(version::tag);
				doc.asset.version = "2.0"; // glTF version, not generator version!

				gltf::Scene scene{};

				for (int i = 0; i < meshToDump.vertexTableCount; ++i) {
					std::vector<uint16_t> indices(meshToDump.faceTables[i].vertCount);
					for (int j = 0; j < meshToDump.faceTables[i].vertCount; ++j)
						indices[j] = meshToDump.faceTables[i].vertices[j];

					std::vector<vec3> positions = meshToDump.vertexTables[i].vertices;

					std::vector<vec3> normals(meshToDump.vertexTables[i].dataCount);
					for (int j = 0; j < meshToDump.vertexTables[i].dataCount; ++j)
						memcpy(&normals[j], &meshToDump.vertexTables[i].normals[j], sizeof(vec3));

					// toss all vectors into a buffer
					// buffer format: indices, positions, normals
					gltf::Buffer buffer{};
					uint32 indexSize = serialize(indices, buffer.data, 0);
					uint32 positionSize = serialize(positions, buffer.data, indexSize);
					uint32 normalSize = serialize(normals, buffer.data, indexSize + positionSize);

					buffer.byteLength = (uint32)buffer.data.size();
					buffer.SetEmbeddedResource();

					// insert into document
					doc.buffers.push_back(buffer);


					// create bufferviews off of the buffers
					gltf::BufferView indexBufferView{};
					gltf::BufferView positionBufferView{};
					gltf::BufferView normalBufferView{};
					indexBufferView.buffer = (int32)doc.buffers.size() - 1;
					indexBufferView.byteLength = (int32)indexSize;
					indexBufferView.byteOffset = 0;
					positionBufferView.buffer = (int32)doc.buffers.size() - 1;
					positionBufferView.byteLength = (int32)positionSize;
					positionBufferView.byteOffset = (int32)indexSize;
					normalBufferView.buffer = (int32)doc.buffers.size() - 1;
					normalBufferView.byteLength = (int32)normalSize;
					normalBufferView.byteOffset = (int32)indexSize + positionSize;

					doc.bufferViews.push_back(indexBufferView);
					doc.bufferViews.push_back(positionBufferView);
					doc.bufferViews.push_back(normalBufferView);


					// make accessors
					gltf::Accessor indexAccessor{};
					gltf::Accessor positionAccessor{};
					gltf::Accessor normalAccessor{};
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


					gltf::Mesh squareMesh{};
					gltf::Primitive squarePrimitive{};
					squareMesh.name = "mesh" + std::to_string(i);
					squarePrimitive.indices = (int32)doc.accessors.size() - 3;  // accessor 0
					squarePrimitive.attributes["POSITION"] = (int32)doc.accessors.size() - 2;  // accessor 1
					squarePrimitive.attributes["NORMAL"] = (int32)doc.accessors.size() - 1;  // accessor 2
					squareMesh.primitives.push_back(squarePrimitive);

					doc.meshes.push_back(squareMesh);


					gltf::Node node{};
					node.mesh = (int32)doc.meshes.size() - 1;

					doc.nodes.push_back(node);
					scene.nodes.push_back((int32)doc.nodes.size() - 1);
				}

				doc.scenes.push_back(scene);
				doc.scene = 0;

				gltf::Save(doc, ofs, outPath.filename().string(), options.OutputFormat == modelSerializerOptions::Format::GLTFBinary);
			}

		} else if (options.OutputFormat == modelSerializerOptions::Format::Dump) {
			// do other things
		}

		ofs.close();
	}

}
}