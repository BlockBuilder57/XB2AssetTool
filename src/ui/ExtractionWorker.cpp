#include "ExtractionWorker.h"


namespace xb2at {
namespace ui {

	//
	// Extraction Worker
	//

	void ExtractionWorker::MakeDirectoryIfNotExists(fs::path& root, const std::string& directoryName) {
		if(directoryName.empty())
			// If the directory name is empty
			// then just assume the user just wants the root directory
			// to be created.
			if(!fs::exists(root))
				fs::create_directory(root);
		else
			// else they want to make a tree.
			// Do that.
			if(!fs::exists(root / directoryName))
				fs::create_directories(root / directoryName);
	}

	// TODO seperate this into different extraction worker functions so that it's less unweildly
	// (at least for people who haven't read the codebase before).
	// Also, because the worker simply emits signals using the logger (no explicit UI action),
	// this may also involve splitting ExtractionWorker into another .cpp file away from this?

	void ExtractionWorker::DoIt(std::string& filename, fs::path& outputPath, ExtractionWorkerOptions& options) {
			using namespace std::placeholders;

			// marshal from C++ -> QT
			auto progress = [&](std::string message, LogSeverity type) {
					Log(QString::fromStdString(message), type);
			};

			// set logger output function
			// (we can't only set it once because we rely on a non-static wrapper,
			// and then we would have an invalid `this` pointer once this function exits. Sucks, I know.)
			Logger::OutputFunction = progress;
			
			logger.info("Input: ", filename);
			logger.info("Output path: ", outputPath.string());
			
			fs::path path(filename);
			std::string filenameOnly = path.stem().string();

			// Make directory tree if it doesn't already exist

			logger.info("Creating output directory tree");
			MakeDirectoryIfNotExists(outputPath, "");

			if (options.saveTextures)
				MakeDirectoryIfNotExists(outputPath, "Textures");

			if(options.saveXBC1)
				MakeDirectoryIfNotExists(outputPath, "Dump");

			path.replace_extension(".wismt");

			if(fs::exists(path)) {
				std::ifstream stream(path.string(), std::ifstream::binary);

				msrdReaderOptions msrdoptions = { outputPath / "Dump", options.saveXBC1 };
				msrdReader msrdreader(stream);
				modelSerializer ms;

				msrd::msrd msrd;
				mesh::mesh mesh;
				mxmd::mxmd mxmd;
				skel::skel skel;
				
				logger.info("Reading MSRD file.");

				msrd = msrdreader.Read(msrdoptions);

				if(msrdoptions.Result != msrdReaderStatus::Success) {
					//VARARGS_LOG(LogSeverity::Error, "Error reading MSRD file: " << msrdReaderStatusToString(msrdoptions.Result))
					logger.error("Error reading MSRD file: ", msrdReaderStatusToString(msrdoptions.Result));
					Done();
					return;
				}

				auto readSAR1 = [&]() {
					sar1::sar1 sar1;

					stream.open(path.string(), std::ifstream::binary);
					sar1Reader sar1reader(stream);
					sar1ReaderOptions sar1options = {};

					sar1 = sar1reader.Read(sar1options);

					stream.close();

					if (sar1options.Result != sar1ReaderStatus::Success) {
						logger.error("Error reading SAR1 file: ", sar1ReaderStatusToString(sar1options.Result));
						return sar1;
					}

					return sar1;
				};

				auto readSKEL = [&]() {
					skel::skel skel;
					sar1::sar1 sar1_skeleton = readSAR1();

					sar1::bc* bcItem;
					for (int i = 0; i < sar1_skeleton.numFiles; i++)
					{
						if (sar1_skeleton.tocItems[i].filename.find(".skl") != std::string::npos)
							bcItem = &sar1_skeleton.bcItems[i];
					}

					if (bcItem == nullptr) {
						logger.error("Skeleton not found in ", path.filename().string(), ", continuing without skeleton...");
						return skel;
					}

					skelReader skelreader;
					skelReaderOptions skeloptions = {(*bcItem).data};

					logger.info("Reading SKEL in ", path.filename().string());
					skel = skelreader.Read(skeloptions);

					if(skeloptions.Result != skelReaderStatus::Success) {
						logger.error("Error reading skeleton, continuing without skeleton...");
					}

					return skel;
				};

				// Close the wismt file and set the extension
				// in preparation of reading the wimdo/MXMD
				stream.close();

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

					}
				}

				msrd.files.clear();
				msrd.textureNames.clear();

				path.replace_extension(".arc");
				// note: this is not how skeleton version differences will be implemented,
				// but it's the only method so far of telling the difference between XC2 and XCDE models w/out asking the user
				if (fs::exists(path)) {
					// we're probably an XC2 model 
					skel = readSKEL();
				} else {
					path.replace_extension(".chr");
					if (fs::exists(path)) {
						// we're probably an XCDE model
						skel = readSKEL();
					} else {
						logger.warn("Skeleton doesn't exist, continuing without skeleton...");
					}
				}
				
				path.replace_extension(".wimdo");

				if (fs::exists(path)) {
					stream.open(path.string(), std::ifstream::binary);
					mxmdReader mxmdreader(stream);
					mxmdReaderOptions mxmdoptions = {};

					mxmd = mxmdreader.Read(mxmdoptions);

					stream.close();

					if (mxmdoptions.Result != mxmdReaderStatus::Success) {
						logger.error("Error reading MXMD file: ", mxmdReaderStatusToString(mxmdoptions.Result));
						Done();
						return;
					}

					logger.info("MXMD file successfully read.");
				} else {
					logger.error(filenameOnly, ".wimdo doesn't exist.");
					Done();
					return;
				}

				modelSerializerOptions msoptions = { options.modelFormat, outputPath, filenameOnly, options.lod, options.saveMorphs, options.saveOutlines };
				ms.Serialize(msrd.meshes, mxmd, skel, msoptions);
			
				msrd.meshes.clear();
				msrd.textures.clear();
				msrd.dataItems.clear();
			} else {
				logger.error(filenameOnly, ".wismt doesn't exist.");
				Done();
				return;
			}

			// Signal successful finish
			logger.info("Extraction successful.");
			Done();
	}


}
}