#include "rio.h"

#include "RootTask.h"

#include "rio-e/PropertyCreatorMgr.h"
#include "rio-e/SceneMgr.h"

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
    rioe::SceneMgr::createSingleton();
    rioe::PropertyCreatorMgr::createSingleton();

    // Initialize RIO with root task
    if (!rio::Initialize<RootTask>(cInitializeArg))
        return -1;

    // Main loop
    rio::EnterMainLoop();

    // Exit RIO
    rio::Exit();

    rioe::SceneMgr::destorySingleton();
    rioe::PropertyCreatorMgr::destorySingleton();

    return 0;
}