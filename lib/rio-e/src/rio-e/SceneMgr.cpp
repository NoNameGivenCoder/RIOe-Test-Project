#include "rio-e/SceneMgr.h"
#include "rio-e/PropertyCreatorMgr.h"

#include "gpu/rio_Drawer.h"
#include "gfx/rio_Window.h"

#include "gfx/rio_PrimitiveRenderer.h"
#include "filedevice/rio_FileDeviceMgr.h"

namespace rioe
{
    SceneMgr* SceneMgr::mInstance = nullptr;

    bool SceneMgr::createSingleton()
    {
        if (mInstance)
            return false;

        mInstance = new SceneMgr();

        return true;
    }

    bool SceneMgr::destroySingleton()
    {
        if (!mInstance)
            return false;

        delete mInstance;
        mInstance = nullptr;

        return true;
    }

    void SceneMgr::Load(LoadArg pLoadArg)
    {
        RIO_LOG("[SceneMgr] Loading from %s..\n", pLoadArg.path.string().c_str());

        rio::FileDevice::LoadArg load;
        load.path = "map/" + pLoadArg.path.string();

        u8* fileBuffer = rio::FileDeviceMgr::instance()->tryLoad(load);

        YAML::Node YAMLNodes = YAML::Load(std::string(reinterpret_cast<char*>(fileBuffer), load.read_size));

        delete fileBuffer;

        mCurrentScene.mNodes.clear();
        mCurrentScene.sceneName = YAMLNodes["sceneName"].as<std::string>();

        // Setting up camera
        rio::Window* window = rio::Window::instance();

        mCurrentScene.mCamera.pos() = { YAMLNodes["camera"]["pos"]["x"].as<f32>(), YAMLNodes["camera"]["pos"]["y"].as<f32>(), YAMLNodes["camera"]["pos"]["z"].as<f32>() };
        mCurrentScene.mCamera.at() = { YAMLNodes["camera"]["at"]["x"].as<f32>(), YAMLNodes["camera"]["at"]["y"].as<f32>(), YAMLNodes["camera"]["at"]["z"].as<f32>() };

        mCurrentScene.mProjection.set(YAMLNodes["camera"]["near"].as<f32>(), YAMLNodes["camera"]["far"].as<f32>(), rio::Mathf::deg2rad(YAMLNodes["camera"]["fov"].as<f32>()), static_cast<f32>(1280) / 720);

        rio::PrimitiveRenderer::instance()->setProjection(mCurrentScene.mProjection); 

        for (YAML::const_iterator it = YAMLNodes["nodes"].begin(); it != YAMLNodes["nodes"].end(); ++it)
        {
            int id = it->first.as<int>();

            YAML::Node node = it->second;

            auto createdNode = std::make_shared<Node>();
            createdNode->SetPosition({ node["transform"]["position"]["x"].as<f32>(), node["transform"]["position"]["y"].as<f32>(), node["transform"]["position"]["z"].as<f32>() });
            createdNode->SetRotation({ node["transform"]["rotation"]["x"].as<f32>(), node["transform"]["rotation"]["y"].as<f32>(), node["transform"]["rotation"]["z"].as<f32>() });
            createdNode->SetScale({ node["transform"]["scale"]["x"].as<f32>(), node["transform"]["scale"]["y"].as<f32>(), node["transform"]["scale"]["z"].as<f32>() });

            createdNode->ID = id;
            createdNode->name = node["name"].as<std::string>();

            for (YAML::const_iterator pt = node["properties"].begin(); pt != node["properties"].end(); ++pt)
            {
                std::string propertyType = pt->first.as<std::string>();
                auto property = PropertyCreatorMgr::instance()->CreateProperty(propertyType);

                if (property)
                {
                    property->Load(pt->second);
                    createdNode->AddProperty(std::move(property));
                }
            }

            mCurrentScene.mNodes.emplace(createdNode->ID, createdNode);

            RIO_LOG("[SceneMgr] Created new node %d.\n", createdNode->ID);
        }
    }
}