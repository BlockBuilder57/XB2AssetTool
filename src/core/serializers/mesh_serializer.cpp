#include <serializers/mesh_serializer.h>
#include <tiny_gltf.h>

namespace xb2at {
namespace core {


	void meshSerializer::Serialize(mesh::mesh& meshToDump, const meshSerializerOptions& options) {
		fs::path outPath(options.outputDir);
		
		outPath = outPath / options.filename;

		if(options.OutputFormat == meshSerializerOptions::Format::GLTFBinary) {
			outPath.replace_extension(".glb");
		} else if (options.OutputFormat == meshSerializerOptions::Format::Dump) {
			outPath.replace_extension(".txt");
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