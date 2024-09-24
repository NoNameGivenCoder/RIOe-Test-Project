#include "RootTask.h"

#include "rio-e/SceneMgr.h"

#include <filedevice/rio_FileDeviceMgr.h>
#include <gfx/rio_Window.h>
#include "rio-e/GLTFHelper.h"

#include "gfx/rio_PrimitiveRenderer.h"

RootTask::RootTask()
    : ITask("RootTask")
{
}

void RootTask::prepare_()
{
    rioe::SceneMgr::instance()->Load({ "./fs/content/map/DefaultScene.yaml", true });

    mMesh = rioe::GLTFRead("./fs/content/models/Cafe.gltf");
    mShader.load("gltf_test", rio::Shader::MODE_UNIFORM_BLOCK);

	rio::MemUtil::copy(&mProjMtx, &rioe::SceneMgr::instance()->GetCurrentScene()->GetPerspectiveProjection()->getMatrix(), sizeof(rio::Matrix44f));

    mModelUniformBlock = new rio::UniformBlock();
    mViewUniformBlock = new rio::UniformBlock();

    // Get uniform locations
    u32 modelLoc = mShader.getVertexUniformBlockIndex("cModelBlock");
    u32 viewLoc = mShader.getVertexUniformBlockIndex("cViewBlock");

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

	rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->pos().set(
		std::sin(mCounter) * 10,
		5,
		std::cos(mCounter) * 10
	);
	mCounter += 1.f / 60;

    mShader.bind();

    // Set the model matrix
    {
        rio::Matrix34f worldMtx; // Assume this is a 4x4 matrix
        worldMtx.makeSRT({ 5, 5, 5 }, { 0, 0, 0 }, { 0, 0, 0 });
        
        rio::Matrix34f normalMtx;

        normalMtx.setInverseTranspose(worldMtx);

        mModelBlock.model_mtx = worldMtx;
        mModelBlock.normal_mtx = normalMtx;

        mModelUniformBlock->setSubDataInvalidate(&mModelBlock, 0, sizeof(ModelBlock));
    }

    // Set the view matrix
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


	mMesh->Draw();
}

void RootTask::exit_()
{
    mShader.unload();

    delete mViewUniformBlock;
    delete mModelUniformBlock;
}