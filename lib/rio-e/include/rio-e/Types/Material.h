#pragma once

#include <string>
#include <vector>

#include "gpu/rio_Texture.h"
#include "gpu/rio_TextureSampler.h"
#include "gpu/rio_Shader.h"
#include "gpu/rio_UniformBlock.h"
#include "gpu/rio_RenderState.h"

#include <memory>

namespace rioe
{
	class Material
	{
	public:
		enum TextureType
		{
			TEXTURE_TYPE_ALBEDO,
			TEXTURE_TYPE_NORMAL,
			TEXTURE_TYPE_SPECULAR,
			TEXTURE_TYPE_ROUGHNESS,
			TEXTURE_TYPE_METALLIC,
			TEXTURE_TYPE_EMISSIVE,
			TEXTURE_TYPE_HEIGHT,
			TEXTURE_TYPE_MAX
		};

		enum AlphaMode
		{
			ALPHA_MODE_OPAQUE,
			ALPHA_MODE_MASK,
			ALPHA_MODE_BLEND
		};
	public:
		Material() : mTextureSamplers(), mTextures(), mTextureShaderLocation()
		{
			
		};

		~Material() 
		{
			for (u32 i = 0; i < TEXTURE_TYPE_MAX; ++i)
				if (mTextures[i])
					delete mTextures[i];

			mShader.unload(); 
		}

		void Bind();

		rio::Shader* GetShader() { return &mShader; };
		void LoadShader(const char* shaderName) 
		{
			mShader.load(shaderName);

			mTextureShaderLocation[TEXTURE_TYPE_ALBEDO] = mShader.getFragmentSamplerLocation("uAlbedoMap");
			mTextureShaderLocation[TEXTURE_TYPE_NORMAL] = mShader.getFragmentSamplerLocation("uNormalMap");
			mTextureShaderLocation[TEXTURE_TYPE_SPECULAR] = mShader.getFragmentSamplerLocation("uSpecularMap");
			mTextureShaderLocation[TEXTURE_TYPE_HEIGHT] = mShader.getFragmentSamplerLocation("uHeightMap");
			mTextureShaderLocation[TEXTURE_TYPE_EMISSIVE] = mShader.getFragmentSamplerLocation("uEmissiveMap");
			mTextureShaderLocation[TEXTURE_TYPE_METALLIC] = mShader.getFragmentSamplerLocation("uMetallicMap");
			mTextureShaderLocation[TEXTURE_TYPE_ROUGHNESS] = mShader.getFragmentSamplerLocation("uRoughnessMap");
		};

		rio::Texture2D* GetTexture(TextureType textureType) { return mTextures[textureType]; };
		rio::TextureSampler2D* GetTextureSampler(TextureType textureType) { return &mTextureSamplers[textureType]; };

		void SetTexture(rio::Texture2D* texture, TextureType textureType) { mTextures[textureType] = texture; };
		void SetTextureLocation(TextureType textureType, u32 fragLocation) { mTextureShaderLocation[textureType] = fragLocation; };

		const std::string& GetName() const { return mName; };

		// Move constructor
		Material(Material&& other) noexcept = default;

		// Move assignment operator
		Material& operator=(Material&& other) noexcept = default;

		// Copy constructor
		Material(const Material& other) = default;

		// Copy assignment operator
		Material& operator=(const Material& other) = default;
		
	private:
		friend class ModelLoader;
		std::string mName;

		rio::Texture2D* mTextures[TEXTURE_TYPE_MAX];
		rio::TextureSampler2D mTextureSamplers[TEXTURE_TYPE_MAX];
		u32 mTextureShaderLocation[TEXTURE_TYPE_MAX];

		rio::Shader mShader;
		rio::RenderState mRenderState;

		AlphaMode mAlphaMode = ALPHA_MODE_OPAQUE;
		bool mUseTexture = true;

		rio::Color4f mAlbedoColor = rio::Color4f::cBlack;
		u32 mAlbedoColorLocation = u32(-1);
	};
}