#include "ExtractionWorker.h"

#include <qmessagebox.h>

#include <modeco/Logger.h>

//#define error(...) error(mco::source_location::current(), ##__VA_ARGS__)
//#define verbose(...) verbose(mco::source_location::current(), ##__VA_ARGS__)

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

			for(int i = 0; i < sar.numFiles; i++) {
				if(sar.tocItems[i].filename.find(".skl") != std::string::npos) {
					bcItem = &sar.bcItems[i];
					break;
				}
			}

			if(!bcItem) {
				return true;
			}

			skelReader skelreader;
			skelReaderOptions skeloptions = { (*bcItem).data };

			logger.info("Reading SKEL in ", path.filename().string());
			skelToReadto = skelreader.Read(skeloptions);

			if(skeloptions.Result != skelReaderStatus::Success) {
				logger.error("Error reading skeleton, continuing without skeleton...");
				return true;
			}

			return true;
		}

		bool ExtractionWorker::ReadMesh(mesh::mesh& mesh, meshReaderOptions& options) {
			meshReader meshreader;
			mesh = meshreader.Read(options);

			if(options.Result != meshReaderStatus::Success)
				return false;

			return true;
		}

		bool ExtractionWorker::ReadMIBL(mibl::texture& texture, miblReaderOptions& options) {
			miblReader miblreader;
			texture = miblreader.Read(options);

			if(options.Result != miblReaderStatus::Success)
				return false;

			return true;
		}

		void ExtractionWorker::SerializeMIBL(fs::path& outputPath, mibl::texture& texture) {
			MIBLDeswizzler deswizzler(texture);
			deswizzler.Deswizzle();

			auto path = outputPath / "Textures" / texture.filename;
			path.replace_extension(".dds");

			deswizzler.Write(path);
		}

		void ExtractionWorker::SerializeMesh(std::vector<mesh::mesh>& meshesToDump, mxmd::mxmd& mxmdData, skel::skel& skelData, modelSerializerOptions& options) {
			modelSerializer ms;
			ms.Serialize(meshesToDump, mxmdData, skelData, options);
		}

		// TODO: This is simplistic enough that I can let this slide
		// but if this becomes any more complex we probably should move this elsewhere
		/**
		 * Mcommon logger sink for the UI.
		 */
		struct UILoggerSink : public mco::Sink {
			UILoggerSink(ExtractionWorker* worker)
				: worker(worker) {
			}

			void Output(const std::string& message, mco::LogSeverity logsev) {
				emit worker->LogMessage(QString::fromStdString(message), logsev);
			}

		   private:
			ExtractionWorker* worker = nullptr;
		};

		void ExtractionWorker::ExtractAll(std::string& filename, fs::path& outputPath, ExtractionWorkerOptions& options) {
			using namespace std::placeholders;

			UILoggerSink sink(this);
			mco::Logger::SetSink(&sink);

#ifdef _DEBUG
			// notify users they're using a development build that could be slower
			logger.info("You're currently using a debug (development) build of XB2AssetTool.");
			logger.info("Performance will be slower, however if something crashes it will be easier to diagnose.");
#endif

			// Output some information about what our input is and where we'll put it.
			logger.info("Input: ", filename);
			logger.info("Output path: ", outputPath.string());

			// Make directory tree if it doesn't already exist

			logger.info("Creating output directory tree");
			MakeDirectoryIfNotExists(outputPath, "");

			if(options.saveTextures)
				MakeDirectoryIfNotExists(outputPath, "Textures");

			if(options.saveXBC1)
				MakeDirectoryIfNotExists(outputPath, "Dump");

			// These are globals used througout the extraction process

			fs::path path(filename);
			std::string filenameOnly = path.stem().string();

			// temporary .xsp test
			/*sar1::sar1 sar;
			sar1ReaderOptions opts = {};

			if (ReadSAR1(path, ".xsp", sar, opts)) {
				logger.info("read good!");

				for (int i = 0; i < sar.numFiles; i++) {
					fs::path filepath = outputPath / "Dump" / sar.tocItems[i].filename;

					logger.info(filepath.c_str());

					std::ofstream stream(filepath, std::ofstream::binary);
					stream.write(&sar.bcItems[i].data[0], sar.bcItems[i].data.size());
					stream.close();
				}
			}
			else
				logger.info("read bad");*/

			msrd::msrd msrd;

			msrdReaderOptions msrdoptions {
				outputPath / "Dump",
				options.saveXBC1
			};

			logger.info("Reading MSRD file.");

			// TODO for asynchronous: the MSRD is the only thing
			// that can't be read async.
			// also should easy api maybe have ReadXXX and ReadXXXAsync() ?

			if(!ReadMSRD(path, msrd, msrdoptions)) {
				logger.error("Error reading MSRD file: ", msrdReaderStatusToString(msrdoptions.Result));
				Done();
				return;
			}

			for(int i = 0; i < msrd.dataItems.size(); ++i) {
				switch(msrd.dataItems[i].type) {
#ifdef _DEBUG
					case msrd::data_item_type::Model: {
						logger.verbose("MSRD file ", i, " is a mesh");
						logger.verbose("Reading mesh ", i, "...");

						mesh::mesh mesh;
						meshReaderOptions meshoptions(msrd.files[i].data);

						if(!ReadMesh(mesh, meshoptions)) {
							logger.error("Error reading mesh from MSRD file ", i, ": ", meshReaderStatusToString(meshoptions.Result));
							Done();
							return;
						}

						msrd.meshes.push_back(mesh);
					} break;
#endif

					case msrd::data_item_type::Texture: {
						logger.verbose("MSRD file ", i, " is a regular MIBL");

						std::string mibl_filename = msrd.textureNames[msrd.textures.size() < msrd.textureInfo.size() ? msrd.textures.size() : msrd.textureIds[msrd.textures.size() % msrd.textureInfo.size()]];

						mibl::texture texture;
						miblReaderOptions mibloptions(msrd.files[1].data, &msrd.files[msrd.dataItems[i].tocIndex - 1]);

						mibloptions.offset = msrd.dataItems[i].offset;
						mibloptions.size = msrd.dataItems[i].size;

#ifdef _DEBUG
						logger.info("regular MIBL name \"", mibl_filename, '\"');
#endif

						if(!ReadMIBL(texture, mibloptions)) {
							logger.error("Error reading MIBL: ", miblReaderStatusToString(mibloptions.Result));
						} else {
							texture.filename = mibl_filename;
							texture.offset = mibloptions.offset;
							texture.size = mibloptions.size;

							msrd.textures.push_back(texture);

							logger.info("MIBL ", i, " successfully read.");
						}

					} break;

					case msrd::data_item_type::CachedTextures: {
						miblReaderOptions mibloptions(msrd.files[0].data, nullptr);

						for(int j = 0; j < msrd.textureCount; ++j) {
							mibloptions.offset = msrd.dataItems[i].offset + msrd.textureInfo[j].offset;
							mibloptions.size = msrd.textureInfo[j].size;

							logger.verbose("MSRD texture ", j, " has a Cached MIBL");

							mibl::texture texture;

#ifdef _DEBUG
							logger.info("Cached MIBL name \"", msrd.textureNames[j], '\"');
#endif

							if(!ReadMIBL(texture, mibloptions)) {
								logger.error("Error reading Cached MIBL: ", miblReaderStatusToString(mibloptions.Result));
							} else {
								texture.offset = mibloptions.offset;
								texture.size = mibloptions.size;
								texture.filename = msrd.textureNames[j];
								msrd.textures.push_back(texture);
								logger.info("Cached MIBL ", j, " successfully read.");
							}
						}
					} break;

					case msrd::data_item_type::ShaderBundle: // We don't care about this quite yet
					default:
						break;
				}
			}

			logger.info("Serializing textures");

			// returns true if a full size texture exists with the same name
			// false otherwise
			auto FullSizeExists = [&](std::string name) {
				auto it = std::find_if(msrd.textures.begin(), msrd.textures.end(), [&name](const mibl::texture& other) {
					return name == other.filename && !other.cached;
				});

				return it != msrd.textures.end();
			};

			for(auto it = msrd.textures.begin(); it != msrd.textures.end(); ++it) {
				if(FullSizeExists((*it).filename)) {
					if(!(*it).cached)
						SerializeMIBL(outputPath, *it);
					else
						logger.verbose("Ignoring ", (*it).filename, "'s cached version because full size one exists");
				} else {
					SerializeMIBL(outputPath, *it);
				}
			}

			// NOTE(lily): this will have to be moved to something awaiting for if/when async stuff happens
			msrd.textures.clear();

			mxmd::mxmd mxmd;
			skel::skel skel;

			if(!ReadSKEL(path, skel)) {
				logger.warn("Continuing without skeletons");
			}

			mxmdReaderOptions mxmdoptions {};

			if(!ReadMXMD(path, mxmd, mxmdoptions)) {
				logger.error("Error reading MXMD file: ", mxmdReaderStatusToString(mxmdoptions.Result));
				Done();
				return;
			}

#ifdef _DEBUG
			modelSerializerOptions msoptions {
				options.modelFormat,
				outputPath,
				filenameOnly,
				options.lod,
				options.saveMorphs,
				options.saveOutlines
			};

			SerializeMesh(msrd.meshes, mxmd, skel, msoptions);
#endif

			// After this point, everything is now unused,
			// so we can clear it all.
			msrd.files.clear();
			msrd.meshes.clear();
			msrd.textures.clear();
			msrd.dataItems.clear();

			// Signal successful finish
			logger.info("Extraction successful.");
			Done();
		}

	} // namespace ui
} // namespace xb2at