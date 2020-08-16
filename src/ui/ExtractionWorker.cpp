#include "ExtractionWorker.h"

namespace xb2at {
namespace ui {


	void ExtractionWorker::MakeDirectoryIfNotExists(fs::path& root, const std::string& directoryName) {
		if(directoryName.empty()) {
			// If the directory name is empty
			// then just assume the user just wants the path itself to be created.
			if(!fs::exists(root))
				fs::create_directory(root);
		} else {
			// Else they want to make a tree with the directory name being the name to create in the root.
			if(!fs::exists(root / directoryName))
				fs::create_directories(root / directoryName);
		}
	}

	void ExtractionWorker::LogCallback(std::string message, LogSeverity type) {
		// Directly emit the signal so that performance isn't garbage
		emit LogMessage(QString::fromStdString(message), type);
	}

	bool ExtractionWorker::ReadMSRD(fs::path& path, msrd::msrd& msrd, msrdReaderOptions& options) {
		path.replace_extension(".wismt");

		if(!fs::exists(path)) {
			logger.error(path.string(), " doesn't exist...");
			return false;
		}
		std::ifstream stream(path.string(), std::ifstream::binary);
		msrdReader reader(stream);

		msrd = reader.Read(options);

		if(options.Result != msrdReaderStatus::Success)
			return false;

		return true;
	}

	bool ExtractionWorker::ReadMXMD(fs::path& path, mxmd::mxmd& mxmdToReadTo, mxmdReaderOptions& options) {
		path.replace_extension(".wimdo");

		if(!fs::exists(path)) {
			logger.error(path.string(), " doesn't exist...");
			return false;
		}

		std::ifstream stream(path.string(), std::ifstream::binary);
		mxmdReader mxmdreader(stream);

		mxmdToReadTo = mxmdreader.Read(options);

		if(options.Result != mxmdReaderStatus::Success)
			return false;

		return true;
	}

	bool ExtractionWorker::ReadSAR1(fs::path& path, const std::string& extension, sar1::sar1& sar1ToReadTo, sar1ReaderOptions& options) {
			path.replace_extension(extension);

			if(!fs::exists(path)) {
				logger.error(path.string(), " doesn't exist... (Possibly not a issue though)");
				return false;
			}

			std::ifstream stream(path.string(), std::ifstream::binary);
			sar1Reader sar1reader(stream);

			sar1ToReadTo = sar1reader.Read(options);


			if(options.Result != sar1ReaderStatus::Success)
				return false;

			return true;
	}

	bool ExtractionWorker::ReadSKEL(fs::path& path, skel::skel& skelToReadto) {
		sar1::sar1 sar;
		
		sar1::bc* bcItem = nullptr;
		sar1ReaderOptions opts = {};
		
		// note: this is not how skeleton version differences will be implemented,
		// but it's the only method so far of telling the difference between XC2 and XCDE models w/out asking the user

		if(!ReadSAR1(path, ".arc", sar, opts)) {
			if(!fs::exists(path)) {
				// assume legitmate failure if we can't read DE .chr file either
				if(!ReadSAR1(path, ".chr", sar, opts)) {
					return false;
				}
			} else {
				// assume legitmate failure
				return false;
			}
		}

		for (int i = 0; i < sar.numFiles; i++) {
			if (sar.tocItems[i].filename.find(".skl") != std::string::npos) {
				bcItem = &sar.bcItems[i];
				break;
			}
		}

		if(!bcItem) {
			return true;
		}

		skelReader skelreader;
		skelReaderOptions skeloptions = {(*bcItem).data};

		logger.info("Reading SKEL in ", path.filename().string());
		skelToReadto = skelreader.Read(skeloptions);

		if(skeloptions.Result != skelReaderStatus::Success) {
			logger.error("Error reading skeleton, continuing without skeleton...");
			return true;
		}

		return true;
			
	}

	void ExtractionWorker::DoIt(std::string& filename, fs::path& outputPath, ExtractionWorkerOptions& options) {
			using namespace std::placeholders;

			// We can't only set it once because we rely on a non-static wrapper. 
			// We would have an invalid bound `this` pointer once this function exits.
			// Sucks, I know.
			Logger::OutputFunction = std::bind(&ExtractionWorker::LogCallback, this, _1, _2);
			
			logger.info("Input: ", filename);
			logger.info("Output path: ", outputPath.string());
			
			fs::path path(filename);
			std::string filenameOnly = path.stem().string();

			msrd::msrd msrd;
			mesh::mesh mesh;
			mxmd::mxmd mxmd;
			skel::skel skel;

			// Make directory tree if it doesn't already exist

			logger.info("Creating output directory tree");
			MakeDirectoryIfNotExists(outputPath, "");

			if(options.saveTextures)
				MakeDirectoryIfNotExists(outputPath, "Textures");

			if(options.saveXBC1)
				MakeDirectoryIfNotExists(outputPath, "Dump");

			msrdReaderOptions msrdoptions { 
				outputPath / "Dump", 
				options.saveXBC1 
			};

			logger.info("Reading MSRD file.");

			if(!ReadMSRD(path, msrd, msrdoptions)) {
				logger.error("Error reading MSRD file: ", msrdReaderStatusToString(msrdoptions.Result));
				Done();
				return;
			}

			// Close the wismt file and set the extension
			// in preparation of reading the wimdo/MXMD

			for(int i = 0; i < msrd.files.size(); ++i) {

				switch(msrd.dataItems[i].type) {

					case msrd::data_item_type::Model: {
						meshReader meshreader;
						meshReaderOptions meshoptions(msrd.files[i].data);
						
						logger.verbose("MSRD file ", i, " is a mesh");
							
						logger.verbose("Reading mesh ", i, "...");
						mesh = meshreader.Read(meshoptions);

						if(meshoptions.Result == meshReaderStatus::Success) {
							msrd.meshes.push_back(mesh);
						} else {
							logger.error("Error reading mesh from MSRD file ", i, ": ", meshReaderStatusToString(meshoptions.Result));
							Done();
							return;
						}
					} break;

					case msrd::data_item_type::Texture: {
						lbimReader lbimreader;
						lbimReaderOptions lbimoptions(msrd.files[1].data, &msrd.files[msrd.dataItems[i].tocIndex == 0 ? 0 : msrd.dataItems[i].tocIndex - 1].data);

						lbimoptions.offset = msrd.dataItems[i].offset;
						lbimoptions.size = msrd.dataItems[i].size;

						logger.verbose("MSRD file ", i, " is a texture");

						lbim::texture texture = lbimreader.Read(lbimoptions);
						texture.filename = msrd.textureNames[i - 2];

						if(lbimoptions.Result != lbimReaderStatus::Success) {
							logger.error("Error reading LBIM: ", lbimReaderStatusToString(lbimoptions.Result));
						} else {
							msrd.textures.push_back(texture);
							logger.info("LBIM ", i, " successfully read.");
						}
					} break;

					case msrd::data_item_type::CachedTextures: {
						lbimReader lbimreader;
						lbimReaderOptions lbimoptions(msrd.files[0].data, nullptr);

						for (int j = 0; j < msrd.textureCount; ++j) {
							lbimoptions.offset = msrd.dataItems[i].offset + msrd.textureInfo[j].offset;
							lbimoptions.size = msrd.textureInfo[j].size;

							logger.verbose("MSRD texture ", j, " has a CachedTexture");

							lbim::texture texture = lbimreader.Read(lbimoptions);
							texture.filename = msrd.textureNames[j];

							if(lbimoptions.Result != lbimReaderStatus::Success) {
								logger.error("Error reading LBIM: ", lbimReaderStatusToString(lbimoptions.Result));
							} else {
								msrd.textures.push_back(texture);
								logger.info("Cached LBIM ", j, " successfully read.");
							}
						}
					} break;

					case msrd::data_item_type::ShaderBundle: // We don't care about this quite yet
					default:
						break;
				}
			}

			// Bit of a memory saving
			//msrd.files.clear();
			//msrd.textureNames.clear();


			if(!ReadSKEL(path, skel)) {
				logger.warn("Continuing without skeletons");
			}
				
			//path.replace_extension(".wimdo");


			mxmdReaderOptions mxmdoptions{};
			
			if(!ReadMXMD(path, mxmd, mxmdoptions)) {
				logger.error("Error reading MXMD file: ", mxmdReaderStatusToString(mxmdoptions.Result));
				Done();
				return;
			}

			modelSerializerOptions msoptions { 
				options.modelFormat, 
				outputPath, 
				filenameOnly, 
				options.lod, 
				options.saveMorphs, 
				options.saveOutlines 
			};

			modelSerializer ms;
			ms.Serialize(msrd.meshes, mxmd, skel, msoptions);
			
			msrd.meshes.clear();
			msrd.textures.clear();
			msrd.dataItems.clear();

			// Signal successful finish
			logger.info("Extraction successful.");
			Done();
	}

}
}