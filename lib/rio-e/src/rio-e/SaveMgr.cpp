#include "rio-e/SaveMgr.h"

namespace rioe
{
    SaveMgr* SaveMgr::mInstance = nullptr;

	bool SaveMgr::createSingleton()
	{
		if (mInstance)
			return false;

		mInstance = new SaveMgr();

		return true;
	}

	bool SaveMgr::destroySingleton()
	{
		if (!mInstance)
			return false;

		delete mInstance;
		mInstance = nullptr;

		return true;
	}


}