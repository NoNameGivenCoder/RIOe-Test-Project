#include "rio-e/Types/Material.h"
#include <math/rio_Vector.h>

#include <rio-e/Common/Time.h>

namespace rioe
{
	void Material::Bind()
	{
		mRenderState.applyDepthAndStencilTest();
		mRenderState.applyBlendAndFastZ();
		mRenderState.applyColorMask();

		mShader.bind();

		for (int i = 0; i < TEXTURE_TYPE_MAX; ++i)
		{
			if (mTextures[i])
			{
				mTextureSamplers[i].tryBindVS(mTextureVertexShaderLocation[i], 1);
				mTextureSamplers[i].tryBindFS(mTextureFragmentShaderLocation[i], 1);
			}
		}
			
		if (mShaderMode != rio::Shader::MODE_UNIFORM_REGISTER)
			return;

		for (int i = 0; i < UNIFORM_TYPE_MAX; ++i)
		{
			switch (i)
			{
			case UNIFORM_TYPE_ROUGHNESS:
				mShader.setUniform((f32)mRoughness, mUniformVertexShaderLocation[i], mUniformFragmentShaderLocation[i]);
				break;
			case UNIFORM_TYPE_METALLIC:
				mShader.setUniform((f32)mMetallic, mUniformVertexShaderLocation[i], mUniformFragmentShaderLocation[i]);
				break;
			case UNIFORM_TYPE_TIME:
				mShader.setUniform(rioe::Time::GetTime(), mUniformVertexShaderLocation[i], mUniformFragmentShaderLocation[i]);
				break;
			}
		}
	}
}