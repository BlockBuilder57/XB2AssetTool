#include <xb2at/readers/sar1_reader.h>
#include <xb2at/streamhelper.h>

namespace xb2at {
	namespace core {

		sar1::sar1 sar1Reader::Read(sar1ReaderOptions& opts) {
			mco::BinaryReader reader(stream);
			sar1::sar1 sar;

			if(!reader.ReadSingleType((sar1::header&)sar)) {
				opts.Result = sar1ReaderStatus::ErrorReadingSAR1Header;
				return sar;
			}

			if(strncmp(sar.magic, "1RAS", sizeof(sar.magic)) != 0) {
				opts.Result = sar1ReaderStatus::NotSAR1;
				return sar;
			}

			sar.path = reader.ReadString();

			sar.tocItems.resize(sar.numFiles);
			for(int i = 0; i < sar.numFiles; i++) {
				stream.seekg(sar.tocOffset + (i * 0x40), std::istream::beg);
				reader.ReadSingleType((sar1::toc_data&)sar.tocItems[i]);
				sar.tocItems[i].filename = reader.ReadString();
			}

			sar.bcItems.resize(sar.numFiles);
			for(int i = 0; i < sar.numFiles; i++) {
				stream.seekg(sar.tocItems[i].offset, std::istream::beg);
				sar1::bc& bcItem = sar.bcItems[i];

				if(!reader.ReadSingleType((sar1::bc_data&)bcItem)) {
					opts.Result = sar1ReaderStatus::ErrorReadingBCHeader;
					return sar;
				}

				if(strncmp(bcItem.magic, "BC\0\0", sizeof(bcItem.magic)) != 0) {
					opts.Result = sar1ReaderStatus::NotBC;
					return sar;
				}

				stream.seekg(sar.tocItems[i].offset + sar.bcItems[i].offsetToData + 0x4, std::istream::beg);

				bcItem.data.resize(bcItem.fileSize);
				stream.read(bcItem.data.data(), bcItem.fileSize);
			}

			opts.Result = sar1ReaderStatus::Success;
			return sar;
		}

	} // namespace core
} // namespace xb2at