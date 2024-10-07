#pragma once

#include "rio.h"

namespace rioe
{
	namespace Endian
	{
		// Returns true if system is big endian, false if not.
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
	}
}