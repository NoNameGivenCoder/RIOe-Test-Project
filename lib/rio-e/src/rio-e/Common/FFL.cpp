#include "rio-e/Common/FFL.h"

#include "filedevice/rio_FileDeviceMgr.h"

namespace rioe
{
	FFLInitDesc FFL::mInitDesc;
	FFLResourceDesc FFL::mResourceDesc;
    FFLMiddleDB FFL::mMiddleDB;
    void* FFL::mMiiBuffer;
    std::vector<FFLStoreData*> FFL::mManagedStoreData;

	void FFL::InitializeFFL()
	{
        mInitDesc.fontRegion = FFL_FONT_REGION_JP_US_EU;
        mInitDesc._c = false;
        mInitDesc._10 = true;

#if RIO_IS_CAFE
        FSInit();
#endif // RIO_IS_CAFE

        {
            std::string resPath;
            resPath.resize(256);
            // Middle
            {
                FFLGetResourcePath(resPath.data(), 256, FFL_RESOURCE_TYPE_MIDDLE, false);
                {
                    rio::FileDevice::LoadArg arg;
                    arg.path = resPath;
                    arg.alignment = 0x2000;

                    u8* buffer = rio::FileDeviceMgr::instance()->getNativeFileDevice()->tryLoad(arg);
                    if (buffer == nullptr)
                    {
                        RIO_LOG("NativeFileDevice failed to load: %s\n", resPath.c_str());
                        RIO_ASSERT(false);
                        return;
                    }

                    mResourceDesc.pData[FFL_RESOURCE_TYPE_MIDDLE] = buffer;
                    mResourceDesc.size[FFL_RESOURCE_TYPE_MIDDLE] = arg.read_size;
                }
            }
            // High
            {
                FFLGetResourcePath(resPath.data(), 256, FFL_RESOURCE_TYPE_HIGH, false);
                {
                    rio::FileDevice::LoadArg arg;
                    arg.path = resPath;
                    arg.alignment = 0x2000;

                    u8* buffer = rio::FileDeviceMgr::instance()->getNativeFileDevice()->tryLoad(arg);
                    if (buffer == nullptr)
                    {
                        RIO_LOG("NativeFileDevice failed to load: %s\n", resPath.c_str());
                        RIO_ASSERT(false);
                        return;
                    }

                    mResourceDesc.pData[FFL_RESOURCE_TYPE_HIGH] = buffer;
                    mResourceDesc.size[FFL_RESOURCE_TYPE_HIGH] = arg.read_size;
                }
            }
        }

        FFLResult result = FFLInitResEx(&mInitDesc, &mResourceDesc);

        if (result == FFL_RESULT_OK)
        {
            RIO_LOG("[RIO(e) FFL] FFLInitResEx() initialized with result: %d\n", (s32)(result));
        } 
        else
        {
            RIO_LOG("[RIO(e) FFL] FFLInitResEx() failed with result: %d\n", (s32)result);

            return;
        }

        FFLiEnableSpecialMii(333326543);
        RIO_ASSERT(FFLIsAvailable());

        FFLInitResGPUStep();
	}

    void FFL::Cleanup()
    {
        FFLExit();

        if (mResourceDesc.pData[FFL_RESOURCE_TYPE_HIGH])
            rio::MemUtil::free(mResourceDesc.pData[FFL_RESOURCE_TYPE_HIGH]);

        if (mResourceDesc.pData[FFL_RESOURCE_TYPE_MIDDLE])
            rio::MemUtil::free(mResourceDesc.pData[FFL_RESOURCE_TYPE_MIDDLE]);

        if (mMiiBuffer)
        {
            rio::MemUtil::free(mMiiBuffer);
            mMiiBuffer = nullptr;
        }

        for (const auto storeDataPtr : mManagedStoreData)
            delete storeDataPtr;

        RIO_LOG("[RIO(e)] FFL Cleaned up!\n");
    }

    void FFL::InitializeRandomDatabase(u16 num)
    {
        if (mMiiBuffer)
            rio::MemUtil::free(mMiiBuffer);
            
        mMiiBuffer = new u8[FFLGetMiddleDBBufferSize(num)];
        FFLInitMiddleDB(&mMiddleDB, FFL_MIDDLE_DB_TYPE_RANDOM_PARAM, mMiiBuffer, num);
        FFLUpdateMiddleDB(&mMiddleDB);

        RIO_LOG("[RIO(e)] Created new random database. Size: %d\n", num);
    }

    FFLStoreData FFL::LoadStoreData(const char* filename)
    {
        rio::FileDevice::LoadArg load;
        load.path = filename;

        u8* data = rio::FileDeviceMgr::instance()->tryLoad(load);

        mManagedStoreData.push_back((FFLStoreData*)data);

        return *(FFLStoreData*)data;
    }
}