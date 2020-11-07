#include <xb2at/readers/skel_reader.h>
#include <xb2at/streamhelper.h>
#include <xb2at/ivstream.h>

namespace xb2at {
	namespace core {

		skel::skel skelReader::Read(skelReaderOptions& opts) {
			ivstream stream(opts.file);
			mco::BinaryReader reader(stream);
			skel::skel skel;

			if(!reader.ReadSingleType((skel::header&)skel)) {
				opts.Result = skelReaderStatus::ErrorReadingHeader;
				return skel;
			}

			if(strncmp(skel.magic, "SKEL", sizeof(skel.magic)) != 0) {
				opts.Result = skelReaderStatus::NotSKEL;
				return skel;
			}

			for(int i = 0; i < ArraySize(skel.tocItems); ++i)
				reader.ReadSingleType((skel::toc&)skel.tocItems[i]);

			stream.seekg(skel.tocItems[skel::Items::NodeParents].offset - sizeof(sar1::bc_data), std::iostream::beg);
			skel.nodeParents.resize(skel.tocItems[skel::Items::NodeParents].count);

			for(int i = 0; i < skel.tocItems[skel::Items::NodeParents].count; ++i)
				reader.ReadSingleType(skel.nodeParents[i]);

			skel.nodes.resize(skel.tocItems[skel::Items::Nodes].count);

			for(int i = 0; i < skel.tocItems[skel::Items::Nodes].count; ++i) {
				stream.seekg(skel.tocItems[skel::Items::Nodes].offset - sizeof(sar1::bc_data) + (i * sizeof(skel::node_data)), std::iostream::beg);
				reader.ReadSingleType((skel::node_data&)skel.nodes[i]);

				stream.seekg(skel.nodes[i].offset - sizeof(sar1::bc_data), std::iostream::beg);
				skel.nodes[i].name = reader.ReadString();
			}

			stream.seekg(skel.tocItems[skel::Items::Transforms].offset - sizeof(sar1::bc_data), std::iostream::beg);
			skel.transforms.resize(skel.tocItems[skel::Items::Transforms].count);

			for(int i = 0; i < skel.tocItems[skel::Items::Transforms].count; ++i)
				reader.ReadSingleType((skel::transform&)skel.transforms[i]);

			opts.Result = skelReaderStatus::Success;

			logger.info("SKEL reading successful");
			return skel;
		}

	} // namespace core
} // namespace xb2at