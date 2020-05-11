#include <serializers/mesh_serializer.h>
#include <tiny_gltf.h>

namespace xb2at {
namespace core {


	void meshSerializer::Serialize(mesh::mesh& meshToDump, const meshSerializerOptions& options) {
		fs::path outPath(options.outputDir);

		if(options.OutputFormat == meshSerializerOptions::Format::GLTFBinary) {
			outPath = outPath / options.filename;
			outPath.replace_extension(".glb");
		}
		
		std::ofstream ofs(outPath.string(), (options.OutputFormat == meshSerializerOptions::Format::GLTFBinary) ? std::ofstream::binary : std::ostream::out);

		if(options.OutputFormat == meshSerializerOptions::Format::GLTFBinary) {
			// do gltf things
		} else if (options.OutputFormat == meshSerializerOptions::Format::Dump) {
			// do other things
		}

		ofs.close();
	}

}
}