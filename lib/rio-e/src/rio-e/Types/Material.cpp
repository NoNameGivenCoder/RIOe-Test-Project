#include "rio-e/Types/Material.h"

namespace rioe
{
	void Material::Bind() const
	{
		mRenderState.apply();
		mShader->bind();

		if (mTextureSampler->isBindable() && mTextureSampler->isTextureAvailable() && mTextureLocation != 0xFFFFFFFF)
			mTextureSampler->bindFS(mTextureLocation, 0);
	}
}