#include "RootTask.h"

#include "rio-e/SceneMgr.h"

#include <filedevice/rio_FileDeviceMgr.h>
#include <gfx/rio_Window.h>
#include "rio-e/ModelLoader.h"
#include <gpu/rio_RenderState.h>

#include "gfx/rio_PrimitiveRenderer.h"

#include "rio-e/Types/Model.h"

RootTask::RootTask()
    : ITask("RootTask")
{
}

void RootTask::prepare_()
{
    RIO_LOG("----RAW BYTES----\n");
    rio::FileDevice::LoadArg load;
    load.path = "./fs/content/models/Cafe.glb";

    rioe::SceneMgr::instance()->Load({ "/vol/content/map/DefaultScene.yaml", true });
    mModel = rioe::ModelLoader::LoadModel("Phone.glb");

	rio::MemUtil::copy(&mProjMtx, &rioe::SceneMgr::instance()->GetCurrentScene()->GetPerspectiveProjection()->getMatrix(), sizeof(rio::Matrix44f));
    
    mModelUniformBlock = new rio::UniformBlock();
    mViewUniformBlock = new rio::UniformBlock();

    mModel->GetMeshes()[0]->GetMaterial()->GetShader()->load("gltf_test");
    
    ////// Get uniform locations
    u32 modelLoc = mModel->GetMeshes()[0]->GetMaterial()->GetShader()->getVertexUniformBlockIndex("cModelBlock");
    u32 viewLoc = mModel->GetMeshes()[0]->GetMaterial()->GetShader()->getVertexUniformBlockIndex("cViewBlock");
    mModel->GetMeshes()[0]->GetMaterial()->SetTextureLocation(mModel->GetMeshes()[0]->GetMaterial()->GetShader()->getFragmentSamplerLocation("texture0"));
    
    mModelUniformBlock->setStage(rio::UniformBlock::STAGE_VERTEX_SHADER);
    mViewUniformBlock->setStage(rio::UniformBlock::STAGE_VERTEX_SHADER);
    
    mModelUniformBlock->setIndex(modelLoc);
    mViewUniformBlock->setIndex(viewLoc);
    
    mViewUniformBlock->setData(&mViewBlock, sizeof(ViewBlock));
    mModelUniformBlock->setData(&mModelBlock, sizeof(ModelBlock));
}

void RootTask::calc_()
{
    rio::Window::instance()->clearColor(0.2f, 0.3f, 0.3f);
    rio::Window::instance()->clearDepthStencil();

	rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->pos().set(
		std::sin(mCounter) * 15,
		10,
		std::cos(mCounter) * 15
	);
	mCounter += 0.8f / 60;

    mModel->GetMeshes()[0]->GetMaterial()->Bind();
    
    //
    //// Set the model matrix
    {
        rio::Matrix34f worldMtx; // Assume this is a 4x4 matrix
        worldMtx.makeSRT({ 3, 3, 3 }, { -30, 0, 0 }, { 0, 5, 0 });
        
        rio::Matrix34f normalMtx;
    
        normalMtx.setInverseTranspose(worldMtx);
    
        mModelBlock.model_mtx = worldMtx;
        mModelBlock.normal_mtx = normalMtx;
    
        mModelUniformBlock->setSubDataInvalidate(&mModelBlock, 0, sizeof(ModelBlock));
    }
    
    //// Set the view matrix
    {
        rio::Matrix44f view_proj_mtx;
        rio::BaseMtx34f viewMtx;
        rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->getMatrix(&viewMtx);
    
        view_proj_mtx.setMul(mProjMtx, viewMtx);
    
        mViewBlock.view_proj_mtx = view_proj_mtx;
        mViewBlock.view_pos = rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->pos();
    
        mViewUniformBlock->setSubDataInvalidate(&mViewBlock, 0, sizeof(ViewBlock));
    }
    
    mViewUniformBlock->bind();
    mModelUniformBlock->bind();
    
    mModel->GetMeshes()[0]->Draw();
}

void RootTask::exit_()
{
    delete mViewUniformBlock;
    delete mModelUniformBlock;
}