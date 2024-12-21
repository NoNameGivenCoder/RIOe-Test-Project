#pragma once

#include "nn/ffl.h"
#include <vector>

namespace rioe
{
	class FFL
	{
	public:
		// Initializes FFL and resources
		static void InitializeFFL();
		// Cleans up FFL and resources
		static void Cleanup();
		// Initializes a random mii database
		static void InitializeRandomDatabase(u16 num);
		// Loads .ffsd from content://[filename]
		static FFLStoreData LoadStoreData(const char* filename);

		static FFLMiddleDB& GetDatabase() { return mMiddleDB; };

	private:
		static FFLInitDesc mInitDesc;
		static FFLResourceDesc mResourceDesc;
		static FFLMiddleDB mMiddleDB;

		static void* mMiiBuffer;

		static std::vector<FFLStoreData*> mManagedStoreData;
	};
	
}