#include "rio-e/Types/Material.h"

namespace rioe
{
	void Material::Bind() const
	{
		mRenderState.apply();
		mShader->bind();

		//for (const auto& texture : mTextures)
		//{
		//	texture.
		//}
	}
}