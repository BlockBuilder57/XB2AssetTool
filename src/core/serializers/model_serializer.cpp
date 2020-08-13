#include <serializers/model_serializer.h>
#include <fx/gltf.h>
#include <glm/mat4x4.hpp>
#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "version.h"

namespace gltf = fx::gltf;
using vec3 = xb2at::core::vector3;

namespace xb2at {
namespace core {

	template <typename T>
	inline uint32 FlattenSerialize(std::vector<T> const& from, std::vector<uint8_t>& to, uint64& offset) {
		uint32 bytesToSerialize = sizeof(T) * (uint32)from.size();

		to.resize(to.size() + bytesToSerialize);
		std::memcpy(&to[offset], &from[0], bytesToSerialize);

		offset += bytesToSerialize;
		return bytesToSerialize;
	}

	struct gltf_defintion {
		uint32 sizeInBuffer;
		uint32 bufferViewIndex;
		uint32 accessorIndex;
	};

	struct node {
		skel::node& node;
		glm::mat4x4 transform;
	};

	template <typename T>
	inline gltf_defintion AddElement(gltf::Document& doc, std::vector<T>& data, gltf::Buffer& buffer, uint64& bufferTally, int32 bufferIndex, gltf::Accessor::ComponentType accessorComponentType, gltf::Accessor::Type accessorType, bool accessorNormalized, std::string accessorName = "") {
		gltf_defintion def;
		
		def.sizeInBuffer = FlattenSerialize(data, buffer.data, bufferTally);

		gltf::BufferView bufferView{};
		bufferView.buffer = bufferIndex;
		bufferView.byteLength = def.sizeInBuffer;
		bufferView.byteOffset = (uint32)bufferTally - def.sizeInBuffer;

		doc.bufferViews.push_back(bufferView);
		def.bufferViewIndex = (uint32)doc.bufferViews.size() - 1;

		gltf::Accessor accessor{};
		accessor.bufferView = def.bufferViewIndex;
		accessor.componentType = accessorComponentType;
		accessor.normalized = accessorNormalized;
		accessor.count = (uint32)data.size();
		accessor.type = accessorType;
		if (!accessorName.empty())
			accessor.name = accessorName;

		doc.accessors.push_back(accessor);
		def.accessorIndex = (uint32)doc.accessors.size() - 1;

		return def;
	}

	void modelSerializer::Serialize(std::vector<mesh::mesh>& meshesToDump, mxmd::mxmd& mxmdData, skel::skel& skelData, modelSerializerOptions& options) {
		fs::path outPath(options.outputDir);
		
		outPath = outPath / options.filename;

		if(options.OutputFormat == modelSerializerOptions::Format::GLTFBinary) {
			outPath.replace_extension(".glb");
		} else if (options.OutputFormat == modelSerializerOptions::Format::GLTFText) {
			outPath.replace_extension(".gltf");
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

				logger.info("Converting mesh ", i, " (", i, '/', mxmdData.Model.meshesCount, ')');

				int32 bonesNodeOffset = doc.nodes.size();

				std::map<std::string, int> SKELNameToNodeIndex;
				for (int k = 0; k < skelData.nodes.size(); ++k) 
					SKELNameToNodeIndex[skelData.nodes[k].name] = k + bonesNodeOffset;

				mesh::mesh& meshToDump = meshesToDump[i];

				mesh::vertex_table weightTbl = meshToDump.vertexTables.back();

				if (options.lod != -1) {
					int lowestLOD = 3;
					int highestLOD = 0;
					for (int j = 0; j < mxmdData.Model.Meshes[i].tableCount; ++j) {
						if (mxmdData.Model.Meshes[i].descriptors[j].lod <= lowestLOD)
							lowestLOD = mxmdData.Model.Meshes[i].descriptors[j].lod;
						else if (mxmdData.Model.Meshes[i].descriptors[j].lod >= highestLOD)
							highestLOD = mxmdData.Model.Meshes[i].descriptors[j].lod;
					}
					if (Clamp(options.lod, lowestLOD, highestLOD) != options.lod) {
						options.lod = Clamp(options.lod, lowestLOD, highestLOD);
						logger.warn("No meshes at chosen LOD level, setting LOD to ", options.lod);
					}
				}

				for (int j = 0; j < mxmdData.Model.Meshes[i].tableCount; ++j) {
					mxmd::mesh_descriptor desc = mxmdData.Model.Meshes[i].descriptors[j];

					if (!options.saveOutlines && mxmdData.Materials.Materials[desc.materialID].name.find("outline") != std::string::npos) 
						continue;

					if (options.lod != -1 && desc.lod != options.lod)
						continue;

					mesh::vertex_table& vertTbl = meshToDump.vertexTables[desc.vertTableIndex];
					mesh::face_table& faceTbl = meshToDump.faceTables[desc.faceTableIndex];

					mesh::morph_descriptor* morphDesc = nullptr;
					
					auto it = Where(meshToDump.morphData.morphDescriptors, [&](mesh::morph_descriptor& meshMorphDesc) {
						return meshMorphDesc.bufferId == desc.vertTableIndex;
					});

					if (it != meshToDump.morphData.morphDescriptors.end()) {
						morphDesc = &(*it);
					}

					//model buffers
					std::vector<uint16_t> indices = faceTbl.vertices;

					std::vector<vec3> positions(vertTbl.vertices.size());
					memcpy(&positions[0], &vertTbl.vertices[0], vertTbl.vertices.size() * sizeof(vec3));

					std::vector<vec3> normals(vertTbl.dataCount);
					for (int k = 0; k < vertTbl.dataCount; ++k) {
						memcpy(&normals[k], &vertTbl.normals[k], sizeof(vec3));
						NormalizeVector3(normals[k]);
					}

					std::vector<color> vertexColors = vertTbl.vertexColor;

					std::vector<vector2> uv0 = vertTbl.uvPos[0];
					std::vector<vector2> uv1 = vertTbl.uvPos[1];
					std::vector<vector2> uv2 = vertTbl.uvPos[2];
					std::vector<vector2> uv3 = vertTbl.uvPos[3];

					std::vector<u16_quaternion> joints(vertTbl.dataCount);
					for (int k = 0; k < vertTbl.dataCount; ++k) {
						joints[k].x = SKELNameToNodeIndex[mxmdData.Model.Skeleton.nodes[weightTbl.weightIds[0][vertTbl.weightTableIndex[k]]].name];
						joints[k].y = SKELNameToNodeIndex[mxmdData.Model.Skeleton.nodes[weightTbl.weightIds[1][vertTbl.weightTableIndex[k]]].name];
						joints[k].z = SKELNameToNodeIndex[mxmdData.Model.Skeleton.nodes[weightTbl.weightIds[2][vertTbl.weightTableIndex[k]]].name];
						joints[k].w = SKELNameToNodeIndex[mxmdData.Model.Skeleton.nodes[weightTbl.weightIds[3][vertTbl.weightTableIndex[k]]].name];
					}

					std::vector<quaternion> weights(vertTbl.dataCount);
					for (int k = 0; k < vertTbl.dataCount; ++k) {
						weights[k] = weightTbl.weightStrengths[vertTbl.weightTableIndex[k]];
					}

					// toss all vectors into a buffer
					// model buffer format: indices, positions, normals, vertexColors, uv0, uv1, uv2, uv3
					gltf::Buffer modelBuffer{};
					uint64 modelBufferTally = 0;

					uint32 buffersCount = (uint32)doc.buffers.size();
					gltf_defintion defIndices =      AddElement(doc, indices,      modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::UnsignedShort, gltf::Accessor::Type::Scalar, false);
					gltf_defintion defPositions =    AddElement(doc, positions,    modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::Float,         gltf::Accessor::Type::Vec3,   false);
					gltf_defintion defNormals =      AddElement(doc, normals,      modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::Float,         gltf::Accessor::Type::Vec3,   false);
					gltf_defintion defVertexColors = AddElement(doc, vertexColors, modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::UnsignedByte,  gltf::Accessor::Type::Vec4,   true);
					gltf_defintion defUV0 =          AddElement(doc, uv0,          modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::Float,         gltf::Accessor::Type::Vec2,   false);
					gltf_defintion defUV1 =          AddElement(doc, uv1,          modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::Float,         gltf::Accessor::Type::Vec2,   false);
					gltf_defintion defUV2 =          AddElement(doc, uv2,          modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::Float,         gltf::Accessor::Type::Vec2,   false);
					gltf_defintion defUV3 =          AddElement(doc, uv3,          modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::Float,         gltf::Accessor::Type::Vec2,   false);
					gltf_defintion defJoints =       AddElement(doc, joints,       modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::UnsignedShort, gltf::Accessor::Type::Vec4,   false);
					gltf_defintion defWeights =      AddElement(doc, weights,      modelBuffer, modelBufferTally, buffersCount, gltf::Accessor::ComponentType::Float,         gltf::Accessor::Type::Vec4,   false);

					modelBuffer.byteLength = (uint32)modelBuffer.data.size();
					if (options.OutputFormat == modelSerializerOptions::Format::GLTFText || j != 0) // "Only 1 buffer, the very first, is allowed to have an empty buffer.uri field.
						modelBuffer.SetEmbeddedResource();

					// insert into document
					doc.buffers.push_back(modelBuffer);

					// viewer requires this
					// indexAccessor.min = { (float)*std::min_element(indices.begin(), indices.end()) };
					// indexAccessor.max = { (float)*std::max_element(indices.begin(), indices.end()) };
					doc.accessors[defIndices.accessorIndex].min = { (float)*std::min_element(indices.begin(), indices.end()) };
					doc.accessors[defIndices.accessorIndex].max = { (float)*std::max_element(indices.begin(), indices.end()) };
					//FIXME ADDME FIXME ADDME FIXME ADDME (eventually)
					//positionAccessor.min = { -0.40553078055381775f, -0.010357379913330078f, -0.43219080567359924f };
					//positionAccessor.max = { -0.24855375289916992f, 0.010233467444777489f, -0.30006110668182373f };
					//normalAccessor.min = { 0, 1, 0 };
					//normalAccessor.max = { 0, 1, 0 };


					// toss all morphs into a buffer
					// morph buffer format: morph0pos, morph0norm, morph1pos, morph1norm, etc.
					gltf::Buffer morphBuffer{};
					uint64 morphBufferTally = 0;
					buffersCount = (uint32)doc.buffers.size();

					std::vector<gltf_defintion> morphPositionDefs;
					std::vector<gltf_defintion> morphNormalDefs;

					// cleanup the buffers we've already commited and don't need at this point
					indices.clear();
					positions.clear();
					normals.clear();
					vertexColors.clear();
					uv0.clear();
					uv1.clear();
					uv2.clear();
					uv3.clear();

					if (options.saveMorphs && morphDesc && morphDesc->targetCounts > 0)
					{
						for (int k = 2; k < meshToDump.morphData.morphTargetCount; ++k)
						{
							int morphId = morphDesc->targetIds[k-2];
							mesh::morph_target& morphTarget = meshToDump.morphData.morphTargets[morphDesc->targetIndex + k];

							std::vector<vec3> morphPositions(vertTbl.vertices.size());
							for (int l = 0; l < vertTbl.vertices.size(); ++l) {
								morphPositions[l].x = morphTarget.vertices[l].x;
								morphPositions[l].y = morphTarget.vertices[l].y;
								morphPositions[l].z = morphTarget.vertices[l].z;
							}

							gltf_defintion defMorphPositions = AddElement(doc, morphPositions, morphBuffer, morphBufferTally, buffersCount, gltf::Accessor::ComponentType::Float, gltf::Accessor::Type::Vec3, false, mxmdData.Model.morphControllers.controls[morphId].name);
							morphPositions.clear();

							std::vector<vec3> morphNormals(vertTbl.vertices.size());
							for (int l = 0; l < vertTbl.vertices.size(); ++l) {
								morphNormals[l].x = morphTarget.normals[l].x;
								morphNormals[l].y = morphTarget.normals[l].y;
								morphNormals[l].z = morphTarget.normals[l].z;
							}

							gltf_defintion defMorphNormals =   AddElement(doc, morphNormals,   morphBuffer, morphBufferTally, buffersCount, gltf::Accessor::ComponentType::Float, gltf::Accessor::Type::Vec3, false, mxmdData.Model.morphControllers.controls[morphId].name);
							morphNormals.clear();

							morphPositionDefs.push_back(defMorphPositions);
							morphNormalDefs.push_back(defMorphNormals);
						}

						morphBuffer.byteLength = (uint32)morphBuffer.data.size();
						morphBuffer.SetEmbeddedResource();

						// insert into document
						doc.buffers.push_back(morphBuffer);
					}
					

					std::string meshName = "mesh" + std::to_string(i) + "_desc" + std::to_string(j);
					if (desc.lod != options.lod)
						meshName += "_LOD" + std::to_string(desc.lod);

					gltf::Mesh gltfMesh{};
					gltf::Primitive gltfPrimitive{};

					gltfMesh.name = meshName;

					gltfPrimitive.material = desc.materialID;
					gltfPrimitive.indices = defIndices.accessorIndex;
					gltfPrimitive.attributes["POSITION"] = defPositions.accessorIndex;
					gltfPrimitive.attributes["NORMAL"] = defNormals.accessorIndex;
					gltfPrimitive.attributes["COLOR_0"] = defVertexColors.accessorIndex;

					if (vertTbl.uvLayerCount > 0) // there's probably a better way to do this
						gltfPrimitive.attributes["TEXCOORD_0"] = defUV0.accessorIndex;
					if (vertTbl.uvLayerCount > 1)
						gltfPrimitive.attributes["TEXCOORD_1"] = defUV1.accessorIndex;
					if (vertTbl.uvLayerCount > 2)
						gltfPrimitive.attributes["TEXCOORD_2"] = defUV2.accessorIndex;
					if (vertTbl.uvLayerCount > 3)
						gltfPrimitive.attributes["TEXCOORD_3"] = defUV3.accessorIndex;

					gltfPrimitive.attributes["JOINTS_0"] = defJoints.accessorIndex;
					gltfPrimitive.attributes["WEIGHTS_0"] = defWeights.accessorIndex;

					if (options.saveMorphs && morphDesc && morphDesc->targetCounts > 0)
					{
						gltfPrimitive.targets.resize(morphPositionDefs.size());

						for (int k = 0; k < morphPositionDefs.size(); ++k)
						{
							gltf::Attributes& morphAttributes = gltfPrimitive.targets[k];
							morphAttributes["POSITION"] = morphPositionDefs[k].accessorIndex;
							morphAttributes["NORMAL"] = morphNormalDefs[k].accessorIndex;
						}
						
						// clear defs
						morphPositionDefs.clear();
						morphNormalDefs.clear();
					}

					gltfMesh.primitives.push_back(gltfPrimitive);

					doc.meshes.push_back(gltfMesh);

					gltf::Node node{};
					node.mesh = (int32)doc.meshes.size() - 1;
					node.skin = (int32)doc.skins.size(); //not size - 1 as we create the skin later

					doc.nodes.push_back(node);
					scene.nodes.push_back((int32)doc.nodes.size() - 1);
				}

				bonesNodeOffset = doc.nodes.size();
				logger.info("Starting to add SKEL to glTF, bonesNodeOffset == ", bonesNodeOffset);

				gltf::Skin skin;
				for (int k = 0; k < skelData.nodes.size(); ++k)
					skin.joints.push_back(k + bonesNodeOffset);
				doc.skins.push_back(skin);

				for (int k = 0; k < skelData.nodes.size(); ++k) {
					gltf::Node node;
					node.name = skelData.nodes[k].name;
					if (skelData.nodeParents[k] != Max<uint16>::value) {
						doc.nodes[skelData.nodeParents[k] + bonesNodeOffset].children.push_back(k + bonesNodeOffset);
						//VARARGS_LOG(LogSeverity::Info, "Node " << skelData.nodes[k].name << " with SKEL Parent of " << skelData.nodeParents[k] << " with a theoritical glTF parent of " << skelData.nodeParents[k] + bonesNodeOffset << " (name: " << doc.nodes[skelData.nodeParents[k] + bonesNodeOffset].name << " at skelData.nodes[" << skelData.nodeParents[k] << "])")
					}
					memcpy(&node.translation, &skelData.transforms[k].position, sizeof(vec3));
					memcpy(&node.scale, &skelData.transforms[k].scale, sizeof(vec3));
					memcpy(&node.rotation, &skelData.transforms[k].rotation, sizeof(quaternion));
					glm::mat4x4 test = MatrixGarbage(skelData.transforms[k].position, skelData.transforms[k].rotation, skelData.transforms[k].scale);

					doc.nodes.push_back(node);
					if (skelData.nodeParents[k] == Max<uint16>::value)
						scene.nodes.push_back((int32)doc.nodes.size() - 1);
				}

				doc.scenes.push_back(scene);
				doc.scene = i;

				logger.info("Writing ", ((options.OutputFormat == modelSerializerOptions::Format::GLTFBinary) ? "Binary" : "Text"), " glTF file to ", outPath.string());

				try {
					gltf::Save(doc, ofs, outPath.filename().string(), options.OutputFormat == modelSerializerOptions::Format::GLTFBinary);
				} catch (gltf::invalid_gltf_document ex) {
					logger.error("fx-glTF exception: ", ex.what());
				} 

				// clear document explicitly
				doc.scenes.clear();
				doc.meshes.clear();
				doc.buffers.clear();
			}

		}
		ofs.close();
	}

}
}