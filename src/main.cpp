#if RIO_IS_WIN
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // RIO_IS_WIN

#include "rio.h"
#include "filedevice/rio_FileDeviceMgr.h"

#include "rio-e/Engine.hpp"

#include "Scenes/TestScene.h"
#include "Scenes/TestScene2.h"

int main()
{
#if RIO_IS_WIN
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // RIO_IS_WIN

    rioe::GetEngine()->AddScene<TestScene>("TestScene");

    rioe::GetEngine()->Start("TestScene");

    rioe::GetEngine()->Exit();

    return 0;
}