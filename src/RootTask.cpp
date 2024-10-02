#include "RootTask.h"

#include "rio-e/SceneMgr.h"

#include <filedevice/rio_FileDeviceMgr.h>
#include <gfx/rio_Window.h>
#include "rio-e/ModelLoader.h"
#include <gpu/rio_RenderState.h>

#include "gfx/rio_PrimitiveRenderer.h"

#include "rio-e/Types/Model.h"

#include "rio-e/Properties/ModelDisplayProperty.hpp"

RootTask::RootTask()
    : ITask("RootTask")
{
}

void RootTask::prepare_()
{
    mModel = rioe::ModelLoader::LoadModel("Bomb2.glb");
    
    rioe::PropertyCreatorMgr::instance()->RegisterProperty("MeshProperty", []() -> std::unique_ptr<rioe::Property> {
        return std::make_unique<rioe::properties::ModelDisplayProperty>();
    });

    rioe::SceneMgr::instance()->Load({ "./fs/content/map/DefaultScene.yaml", true });

    auto modelProperty = rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(5)->GetProperty<rioe::properties::ModelDisplayProperty>();

    modelProperty[0]->SetDisplayModel(mModel);
}

void RootTask::calc_()
{
    rio::Window::instance()->clearColor(0.2f, 0.3f, 0.3f);
    rio::Window::instance()->clearDepthStencil();

	rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->pos().set(
		std::sin(mCounter) * 10,
		15,
		std::cos(mCounter) * 10
	);
	mCounter += 0.8f / 60;

    auto modelProperty = rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(5)->GetProperty<rioe::properties::ModelDisplayProperty>();
    modelProperty[0]->Update();
}

void RootTask::exit_()
{

}