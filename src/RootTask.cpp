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
    mShader.load("gltf_test", rio::Shader::MODE_UNIFORM_REGISTER);

	rio::MemUtil::copy(&mProjMtx, &rioe::SceneMgr::instance()->GetCurrentScene()->GetPerspectiveProjection()->getMatrix(), sizeof(rio::Matrix44f));
}

void RootTask::calc_()
{
    rio::Window::instance()->clearColor(0.2f, 0.3f, 0.3f);

	rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->pos().set(
		std::sin(mCounter) * 10,
		std::sin(mCounter) * 10,
		std::cos(mCounter) * 10
	);
	mCounter += 1.f / 60;

    mShader.bind();

    // Get uniform locations
    u32 modelLoc = mShader.getVertexUniformLocation("model");
    u32 viewLoc = mShader.getVertexUniformLocation("view");
    u32 projectionLoc = mShader.getVertexUniformLocation("projection");

    // Set the model matrix
    {
        rio::Matrix44f modelMtx; // Assume this is a 4x4 matrix
        modelMtx.makeSRT({ 5, 5, 5 }, { 0, 0, 0 }, { 0, 0, 0 });
        mShader.setUniform(modelMtx, modelLoc, u32(-1));
    }

    // Set the view matrix
    {
        rio::Matrix44f viewMtx44;
        rio::BaseMtx34f viewMtx;
        rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->getMatrix(&viewMtx);

        viewMtx44.fromMatrix34(viewMtx);
        mShader.setUniform(viewMtx44, viewLoc, u32(-1));
    }

    // Set the projection matrix
    {
        mShader.setUniform(mProjMtx, projectionLoc, u32(-1));
    }

	mMesh->Draw();
}

void RootTask::exit_()
{
    mShader.unload();
}