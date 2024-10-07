#if RIO_IS_WIN
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // RIO_IS_WIN

#include "rio.h"

#include "RootTask.h"

#include "rio-e/PropertyCreatorMgr.h"
#include "rio-e/SceneMgr.h"
#include "rio-e/ModelLoader.h"

static const rio::InitializeArg cInitializeArg = {
    .window = {
#if RIO_IS_WIN
        .gl_major = 4,
        .gl_minor = 3
#endif // RIO_IS_WIN
    }
};

int main()
{
#if RIO_IS_WIN
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // RIO_IS_WIN

    rioe::SceneMgr::createSingleton();
    rioe::PropertyCreatorMgr::createSingleton();

    // Initialize RIO with root task
    if (!rio::Initialize<RootTask>(cInitializeArg))
        return -1;

    // Main loop
    rio::EnterMainLoop();

    rioe::SceneMgr::destroySingleton();
    rioe::PropertyCreatorMgr::destroySingleton();
    rioe::ModelLoader::Cleanup();
    // Exit RIO
    rio::Exit();

    return 0;
}