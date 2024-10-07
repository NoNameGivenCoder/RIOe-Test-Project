#include "RootTask.h"

#include "rio-e/SceneMgr.h"

#include <gfx/rio_Window.h>
#include "rio-e/ModelLoader.h"

#include "rio-e/Types/Model.h"

#include "rio-e/Properties/ModelDisplayProperty.hpp"
#include "rio-e/Properties/DebugDisplayProperty.hpp"

RootTask::RootTask()
    : ITask("RootTask")
{
}

void RootTask::prepare_()
{   
    rioe::PropertyCreatorMgr::instance()->RegisterProperty("MeshProperty", []() -> std::unique_ptr<rioe::Property> {
        return std::make_unique<rioe::properties::ModelDisplayProperty>();
    });

    rioe::PropertyCreatorMgr::instance()->RegisterProperty("DebugProperty", []() -> std::unique_ptr<rioe::Property> {
        return std::make_unique<rioe::properties::DebugDisplayProperty>();
    });

    rioe::SceneMgr::instance()->Load({ "DefaultScene.yaml", true });

    //auto modelProperty = rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(5)->GetProperty<rioe::properties::ModelDisplayProperty>();
    //auto modelProperty2 = rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(7)->GetProperty<rioe::properties::ModelDisplayProperty>();

    //modelProperty2[0]->SetDisplayModel(rioe::ModelLoader::LoadModel("Bomb.glb"));
    //modelProperty[0]->SetDisplayModel(rioe::ModelLoader::LoadModel("Bomb2.glb"));

    mSkeleton = rioe::ModelLoader::LoadSkeleton("BaseMiiBody.glb");

    rio::MemUtil::copy(&mProjMtx, &rioe::SceneMgr::instance()->GetCurrentScene()->GetPerspectiveProjection()->getMatrix(), sizeof(rio::Matrix44f));

    uniformBlockArray = new rio::UniformBlock[mSkeleton->GetMeshes().size()];
    skeletalBlockArray = new SkeletalBlock[mSkeleton->GetMeshes().size()];

    for (int i = 0; i < mSkeleton->GetMeshes().size(); i++)
    {
        const auto& mesh = mSkeleton->GetMeshes()[i];
        mesh->GetMaterial()->GetShader()->load("skinning_demo");
        
        uniformBlockArray[i].setStage(rio::UniformBlock::STAGE_VERTEX_SHADER);
        uniformBlockArray[i].setIndex(mesh->GetMaterial()->GetShader()->getVertexUniformBlockIndex("SkeletalBlock"));
        uniformBlockArray[i].setDataInvalidate(&skeletalBlockArray[i], sizeof(SkeletalBlock));

        mesh->GetMaterial()->SetTextureLocation(mesh->GetMaterial()->GetShader()->getFragmentSamplerLocation("texture0"));
    }

    mNode = rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(3);
    //mNode->AddProperty(rioe::PropertyCreatorMgr::instance()->CreateProperty("DebugProperty"));
}

void RootTask::calc_()
{
    rio::Window::instance()->clearColor(0.2f, 0.3f, 0.3f);
    rio::Window::instance()->clearDepthStencil();

	rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->pos().set(
        std::sin(mCounter) * 0.3,
        0.3f,
        std::cos(mCounter) * 0.3
	);
	mCounter += 0.2f / 60;

    mNode->SetRotation({ 0, 0, 1 });

    rioe::SceneMgr::instance()->GetCurrentScene()->Update();

    // Set the uniform array for each mesh
    for (int i = 0; i < mSkeleton->GetMeshes().size(); i++) {
        const auto& mesh = mSkeleton->GetMeshes()[i];

        mesh->GetMaterial()->Bind();

        rio::Matrix44f view_proj_mtx;
        rio::BaseMtx34f viewMtx;
        rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->getMatrix(&viewMtx);

        view_proj_mtx.setMul(mProjMtx, viewMtx);

        skeletalBlockArray[i].projection_matrix = view_proj_mtx;
        
        for (int s = 0; s < mSkeleton->GetBones().size(); s++)
        {
            rio::Matrix34f worldMtx;
            mSkeleton->GetBones()[s]->GetWorldMatrix(&worldMtx);

            rio::Matrix44f boneMatrix = rio::Matrix44f::ident;

            boneMatrix.fromMatrix34(worldMtx);

            skeletalBlockArray[i].boneMatrices[s] = boneMatrix;
        }

        uniformBlockArray[i].setSubDataInvalidate(&skeletalBlockArray[i], 0, sizeof(SkeletalBlock));
        uniformBlockArray[i].bind();

        mesh->Draw();
    }
}

void RootTask::exit_()
{
    delete[] uniformBlockArray;
    delete[] skeletalBlockArray;
}