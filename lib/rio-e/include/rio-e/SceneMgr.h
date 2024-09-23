#pragma once

#include "rio.h"
#include "math/rio_MathTypes.h"
#include "math/rio_Matrix.h"

#include "gfx/rio_Camera.h"

#include "rio-e/PropertyCreatorMgr.h"
#include "rio-e/Types/Scene.h"

#include "yaml-cpp/yaml.h"

#include <unordered_map>
#include <memory>
#include <filesystem>
#include <functional>

namespace rioe
{
    class SceneMgr
    {
    public:
        struct LoadArg
        {
            std::filesystem::path path;
            bool reloadScene = true;
        };
    public:
        static bool createSingleton();
        static bool destorySingleton();

        static inline SceneMgr* instance() { return mInstance; };

        void Load(LoadArg pLoadArg);

        inline Scene* GetCurrentScene() { return &mCurrentScene; };
        
    private:
        Scene mCurrentScene;

        static SceneMgr* mInstance;
    };
}
