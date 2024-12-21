#include "rio-e/Types/UI/UIElement.h"
#include "gpu/rio_Texture.h"
#include "gpu/rio_RenderState.h"

namespace rioe
{
	UIElement::UIElement()
	{
        mVertices.reserve(6);

        mVertices = {
            {{ 0.0f, 1.0f }, { 0.0f, 1.0f }},  // bottom-left
            {{ 1.0f, 0.0f }, { 1.0f, 0.0f }},  // top-right
            {{ 0.0f, 0.0f }, { 0.0f, 0.0f }},  // top-left

            {{ 0.0f, 1.0f }, { 0.0f, 1.0f }},  // bottom-left
            {{ 1.0f, 1.0f }, { 1.0f, 1.0f }},  // bottom-right
            {{ 1.0f, 0.0f }, { 1.0f, 0.0f }}   // top-right
        };

        mVBO.setStride(sizeof(UIVertex));
        mVBO.setDataInvalidate(mVertices.data(), mVertices.size() * sizeof(UIVertex));

        mPosStream.setLayout(0, rio::VertexStream::FORMAT_32_32_FLOAT, offsetof(UIVertex, pos));
        mTexCoordStream.setLayout(1, rio::VertexStream::FORMAT_32_32_FLOAT, offsetof(UIVertex, tex_coord));

        mVAO.initialize();
        mVAO.addAttribute(mPosStream, mVBO);
        mVAO.addAttribute(mTexCoordStream, mVBO);
        mVAO.process();

        mShader.load("ui_base_shader");
        mTransformMatrixLocation = mShader.getVertexUniformLocation("model");
        mProjectionMatrixLocation = mShader.getVertexUniformLocation("projection");
        mTransparencyLocation = mShader.getFragmentUniformLocation("transparency");

        //rio::MemUtil::copy(&mProjMtx, &rioe::SceneMgr::instance()->GetCurrentScene()->GetOrthoProjection()->getMatrix(), sizeof(rio::Matrix44f));
	}

    void UIElement::SetShader(char* shaderName)
    {
        if (mShader.isLoaded())
            mShader.unload();

        mShader.load(shaderName);

        mTextureLocation = mShader.getFragmentSamplerLocation("texture0");
        mTransformMatrixLocation = mShader.getVertexUniformLocation("model");
        mProjectionMatrixLocation = mShader.getVertexUniformLocation("projection");
        mTransparencyLocation = mShader.getFragmentUniformLocation("transparency");
    }

    void UIElement::SetTexture(char* textureName, bool scaleWithSize)
    {
        if (isTextureLoaded)
            delete mTexture;

        mTexture = new rio::Texture2D(textureName);
        mTextureSampler.linkTexture2D(mTexture);
        mTextureLocation = mShader.getFragmentSamplerLocation("texture0");

        isTextureLoaded = true;

        if (scaleWithSize)
            SetScale({ static_cast<f32>(mTexture->getWidth()), static_cast<f32>(mTexture->getHeight()), 1.0f });
    }

    void UIElement::Draw()
    {
        rio::RenderState renderState;
        renderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);
        renderState.apply();

        mShader.bind();
        mTextureSampler.bindFS(mTextureLocation, 0);

        if (mDirtyTransform)
        {
            mTransformMatrix.makeSRT(mScreenScale, mScreenRotation, { mScreenPosition.x, mScreenPosition.y, 0.0f });
            mDirtyTransform = false;
        }

        mShader.setUniform(mTransformMatrix, mTransformMatrixLocation, u32(-1));
        mShader.setUniform(mProjMtx, mProjectionMatrixLocation, u32(-1));
        mShader.setUniform(mTransparency, u32(-1), mTransparencyLocation);

        mVAO.bind();
        rio::Drawer::DrawArrays(rio::Drawer::TRIANGLES, mVertices.size(), 0);
    }
}