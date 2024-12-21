#pragma once

#include "rio.h"

namespace rioe
{
	namespace Endian
	{
		// Returns true if system is big endian, false if not (Wii U).
		bool BigEndian() {
			int num = 1;
			return *(char*)&num == 0;
		}

		u32 SwapEndianness(u32 value) {
			return ((value >> 24) & 0xff) |
				((value << 8) & 0xff0000) |
				((value >> 8) & 0xff00) |
				((value << 24) & 0xff000000);
		}

		float SwapEndianness(float value) {
			uint32_t intRep;
			// Use memcpy to avoid strict aliasing issues
			rio::MemUtil::copy(&intRep, &value, sizeof(float));
			intRep = SwapEndianness(intRep);
			float swapped;
			rio::MemUtil::copy(&swapped, &intRep, sizeof(float));
			return swapped;
		}

		unsigned short SwapEndianness(unsigned short value) {
			return (value >> 8) | (value << 8);
		}

		rio::Vector3f SwapEndianness(rio::Vector3f value)
		{
			value.x = SwapEndianness(value.x);
			value.y = SwapEndianness(value.y);
			value.z = SwapEndianness(value.z);

			return value;
		}

		rio::Quatf SwapEndianness(rio::Quatf value)
		{
			value.x = SwapEndianness(value.x);
			value.y = SwapEndianness(value.y);
			value.z = SwapEndianness(value.z);
			value.w = SwapEndianness(value.w);

			return value;
		}

		rio::BaseVec3f SwapEndianness(rio::BaseVec3f value)
		{
			value.x = SwapEndianness(value.x);
			value.y = SwapEndianness(value.y);
			value.z = SwapEndianness(value.z);

			return value;
		}

		rio::BaseQuatf SwapEndianness(rio::BaseQuatf value)
		{
			value.x = SwapEndianness(value.x);
			value.y = SwapEndianness(value.y);
			value.z = SwapEndianness(value.z);
			value.w = SwapEndianness(value.w);

			return value;
		}
	}
}