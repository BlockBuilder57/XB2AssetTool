#pragma once
#include <core.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <modeco/BinaryReader.h>

namespace xb2at {
	namespace core {

		/**
		 * Read a Vector2 comprised of floats.
		 * Returns said Vector2.
		 */
		inline vector2 ReadVec2(mco::BinaryReader& reader) {
			float x;
			float y;

			reader.ReadSingleType(x);
			reader.ReadSingleType(y);

			return { x, y };
		}

		/**
		 * Read a Vector3 comprised of floats.
		 * Returns said Vector3.
		 */
		inline vector3 ReadVec3(mco::BinaryReader& reader) {
			float x;
			float y;
			float z;

			reader.ReadSingleType(x);
			reader.ReadSingleType(y);
			reader.ReadSingleType(z);

			return { x, y, z };
		}

		/**
		 * Read a quaternion comprised of floats.
		 * Returns said quaternion.
		 */
		inline quaternion ReadQuaternion(mco::BinaryReader& reader) {
			float x;
			float y;
			float z;
			float w;

			reader.ReadSingleType(x);
			reader.ReadSingleType(y);
			reader.ReadSingleType(z);
			reader.ReadSingleType(w);

			return { x, y, z, w };
		}

		/**
		 * Read a quaternion comprised of 4 signed 8-bit values.
		 * Returns said quaternion.
		 */
		inline quaternion ReadS8Quaternion(mco::BinaryReader& reader) {
			uint32 total;
			reader.ReadSingleType(total);

			float x = (float)(((sbyte*)&total)[0]) / 128.f;
			float y = (float)(((sbyte*)&total)[1]) / 128.f;
			float z = (float)(((sbyte*)&total)[2]) / 128.f;
			float w = (float)(((sbyte*)&total)[3]);

			return { x, y, z, w };
		}

		/**
		 * Read a quaternion comprised of 4 unsigned 8-bit values.
		 * Returns said quaternion.
		 */
		inline quaternion ReadU8Quaternion(mco::BinaryReader& reader) {
			uint32 total;
			reader.ReadSingleType(total);

			float x = (float)(((byte*)&total)[0]) / 128.f;
			float y = (float)(((byte*)&total)[1]) / 128.f;
			float z = (float)(((byte*)&total)[2]) / 128.f;
			float w = (float)(((byte*)&total)[3]);

			return { x, y, z, w };
		}

		/**
		 * Read a quaternion comprised of 4 unsigned 16-bit values.
		 * Returns said quaternion.
		 */
		inline quaternion ReadU16Quaternion(mco::BinaryReader& reader) {
			uint64 total;
			reader.ReadSingleType(total);

			float x = (float)(((uint16*)&total)[0]) / 65535.f;
			float y = (float)(((uint16*)&total)[1]) / 65535.f;
			float z = (float)(((uint16*)&total)[2]) / 65535.f;
			float w = (float)(((uint16*)&total)[3]) / 65535.f;

			return { x, y, z, w };
		}

	} // namespace core
} // namespace xb2at