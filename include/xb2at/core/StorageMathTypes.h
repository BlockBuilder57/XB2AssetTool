//
// Created by block on 7/22/2021.
//

#ifndef XB2AT_STORAGEMATHTYPES_H
#define XB2AT_STORAGEMATHTYPES_H

#include <xb2at/core/Stream.h>

namespace xb2at::core {

	/**
	 * Vector2 of floats, used only for reading into.
	 */
	struct vector2 {
		float x;
		float y;

		template<core::Stream Stream>
		inline bool Transform(Stream &stream) {
			XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(x));
			XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(y));
			return true;
		}
	};

	/**
	 * Vector3 of floats, used only for reading into.
	 */
	struct vector3 {
		float x;
		float y;
		float z;

		template<core::Stream Stream>
		inline bool Transform(Stream &stream) {
			XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(x));
			XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(y));
			XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(z));
			return true;
		}

		/**
 		 * Normalize this vector.
 	 	 * Returns copy-elided normalized vector.
 	 	 *
 		 * \param[in] vector Vector to normalize.
 		 */
		inline vector3 Normalized() {
			const double mag = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
			return {
				x / (float)mag,
				y / (float)mag,
				z / (float)mag
			};
		}
	};

	/**
	 * Quaternion of floats, used only for reading into.
	 */
	struct quaternion {
		float x;
		float y;
		float z;
		float w;

		enum class QuatType {
			Float,
			S8Quat,
			U8Quat,
			U16Quat
		};

		template<core::Stream Stream, QuatType Type>
		inline bool Transform(Stream &stream) {
			switch(type) {
				case QuatType::Float:
					XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(x));
					XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(y));
					XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(z));
					XB2AT_TRANSFORM_CATCH(stream.template Float<std::endian::little>(w));
					break;

				case QuatType::S8Quat: {
					static_assert(Stream::IsReadStream(), "don't use this quaternion type with a write stream yet");
					std::uint32_t total;
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(total));
					x = (float)(((std::int8_t *)&total)[0]) / 128.f;
					y = (float)(((std::int8_t *)&total)[1]) / 128.f;
					z = (float)(((std::int8_t *)&total)[2]) / 128.f;
					w = (float)(((std::int8_t *)&total)[3]);
				} break;

				case QuatType::U8Quat: {
					static_assert(Stream::IsReadStream(), "don't use this quaternion type with a write stream yet");
					std::uint32_t total;
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(total));
					x = (float)(((std::uint8_t *)&total)[0]) / 128.f;
					y = (float)(((std::uint8_t *)&total)[1]) / 128.f;
					z = (float)(((std::uint8_t *)&total)[2]) / 128.f;
					w = (float)(((std::uint8_t *)&total)[3]);
				} break;

				case QuatType::U16Quat: {
					static_assert(Stream::IsReadStream(), "don't use this quaternion type with a write stream yet");
					std::uint64_t total;
					XB2AT_TRANSFORM_CATCH(stream.template Uint64<std::endian::little>(total));

					x = (float)(((std::uint16_t *)&total)[0]) / 65535.f;
					y = (float)(((std::uint16_t *)&total)[1]) / 65535.f;
					z = (float)(((std::uint16_t *)&total)[2]) / 65535.f;
					w = (float)(((std::uint16_t *)&total)[3]) / 65535.f;
				} break;
			}

			return true;
		}
	};

	/**
	 * Quaternion of u16's, used only for reading into.
	 */
	struct u16_quaternion {
		std::uint16_t x;
		std::uint16_t y;
		std::uint16_t z;
		std::uint16_t w;

		template<core::Stream Stream>
		inline bool Transform(Stream &stream) {
			XB2AT_TRANSFORM_CATCH(stream.template Uint16<std::endian::little>(x));
			XB2AT_TRANSFORM_CATCH(stream.template Uint16<std::endian::little>(y));
			XB2AT_TRANSFORM_CATCH(stream.template Uint16<std::endian::little>(z));
			XB2AT_TRANSFORM_CATCH(stream.template Uint16<std::endian::little>(w));
			return true;
		}
	};

	/**
	 * RGBA color
	 */
	struct Rgba32 {
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t a;

		template<core::Stream Stream>
		inline bool Transform(Stream &stream) {
			XB2AT_TRANSFORM_CATCH(stream.Byte(r));
			XB2AT_TRANSFORM_CATCH(stream.Byte(g));
			XB2AT_TRANSFORM_CATCH(stream.Byte(b));
			XB2AT_TRANSFORM_CATCH(stream.Byte(a));
			return true;
		}
	};



} // namespace xb2at::core

#endif //XB2AT_STORAGEMATHTYPES_H
