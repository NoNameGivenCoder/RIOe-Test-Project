#include "rio-e/Types/Material.h"
#include <math/rio_Vector.h>

namespace rioe
{
	void Material::Bind()
	{
		mRenderState.applyDepthAndStencilTest();
		mRenderState.applyBlendAndFastZ();
		mRenderState.applyColorMask();

		mShader.bind();

		if (mUseTexture)
		{
			for (int i = 0; i < TEXTURE_TYPE_MAX; ++i)
				if (mTextures[i])
					mTextureSamplers[i].tryBindFS(mTextureShaderLocation[i], 1);
		}
		else
		{
			mShader.setUniform(mAlbedoColor.v, u32(-1), mAlbedoColorLocation);
		}
	}
}