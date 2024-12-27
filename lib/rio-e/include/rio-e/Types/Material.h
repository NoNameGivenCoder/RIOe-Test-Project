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

		enum UniformType
		{
			UNIFORM_TYPE_ROUGHNESS,
			UNIFORM_TYPE_METALLIC,
			UNIFORM_TYPE_TIME,
			UNIFORM_TYPE_PROJECTION_VIEW_MATRIX,
			UNIFORM_TYPE_MODEL_MATRIX,
			UNIFORM_TYPE_MAX
		};
	public:
		Material() : mTextureSamplers(), mTextures()
		{
			
		};

		~Material() 
		{
			for (u32 i = 0; i < TEXTURE_TYPE_MAX; ++i)
				if (mTextures[i])
					delete mTextures[i];

			if (mShader.isLoaded())
				mShader.unload(); 
		}

		void Bind();

		void SetShaderMode(rio::Shader::ShaderMode shaderMode) { mShader.setShaderMode(shaderMode); mShaderMode = shaderMode; };
		const rio::Shader::ShaderMode GetShaderMode() const { return mShaderMode; };

		rio::Shader* GetShader() { return &mShader; };
		void LoadShader(const char* shaderName) 
		{
			if (mShader.isLoaded())
				mShader.unload();

			mShader.load(shaderName);

			// Textures
			{
				mTextureFragmentShaderLocation[TEXTURE_TYPE_ALBEDO] = mShader.getFragmentSamplerLocation("uAlbedoMap");
				mTextureFragmentShaderLocation[TEXTURE_TYPE_NORMAL] = mShader.getFragmentSamplerLocation("uNormalMap");
				mTextureFragmentShaderLocation[TEXTURE_TYPE_SPECULAR] = mShader.getFragmentSamplerLocation("uSpecularMap");
				mTextureFragmentShaderLocation[TEXTURE_TYPE_HEIGHT] = mShader.getFragmentSamplerLocation("uHeightMap");
				mTextureFragmentShaderLocation[TEXTURE_TYPE_EMISSIVE] = mShader.getFragmentSamplerLocation("uEmissiveMap");
				mTextureFragmentShaderLocation[TEXTURE_TYPE_METALLIC] = mShader.getFragmentSamplerLocation("uMetallicMap");
				mTextureFragmentShaderLocation[TEXTURE_TYPE_ROUGHNESS] = mShader.getFragmentSamplerLocation("uRoughnessMap");

				mTextureVertexShaderLocation[TEXTURE_TYPE_ALBEDO] = mShader.getVertexSamplerLocation("uAlbedoMap");
				mTextureVertexShaderLocation[TEXTURE_TYPE_NORMAL] = mShader.getVertexSamplerLocation("uNormalMap");
				mTextureVertexShaderLocation[TEXTURE_TYPE_SPECULAR] = mShader.getVertexSamplerLocation("uSpecularMap");
				mTextureVertexShaderLocation[TEXTURE_TYPE_HEIGHT] = mShader.getVertexSamplerLocation("uHeightMap");
				mTextureVertexShaderLocation[TEXTURE_TYPE_EMISSIVE] = mShader.getVertexSamplerLocation("uEmissiveMap");
				mTextureVertexShaderLocation[TEXTURE_TYPE_METALLIC] = mShader.getVertexSamplerLocation("uMetallicMap");
				mTextureVertexShaderLocation[TEXTURE_TYPE_ROUGHNESS] = mShader.getVertexSamplerLocation("uRoughnessMap");
			}

			// Uniforms
			if (mShaderMode == rio::Shader::MODE_UNIFORM_REGISTER)
			{
				mUniformVertexShaderLocation[UNIFORM_TYPE_ROUGHNESS] = mShader.getVertexUniformLocation("uRoughness");
				mUniformVertexShaderLocation[UNIFORM_TYPE_METALLIC] = mShader.getVertexUniformLocation("uMetallic");
				mUniformVertexShaderLocation[UNIFORM_TYPE_TIME] = mShader.getVertexUniformLocation("uTime");
				mUniformVertexShaderLocation[UNIFORM_TYPE_MODEL_MATRIX] = mShader.getVertexUniformLocation("uModelMatrix");
				mUniformVertexShaderLocation[UNIFORM_TYPE_PROJECTION_VIEW_MATRIX] = mShader.getVertexUniformLocation("uProjectionViewMatrix");

				mUniformFragmentShaderLocation[UNIFORM_TYPE_ROUGHNESS] = mShader.getFragmentUniformLocation("uRoughness");
				mUniformFragmentShaderLocation[UNIFORM_TYPE_METALLIC] = mShader.getFragmentUniformLocation("uMetallic");
				mUniformFragmentShaderLocation[UNIFORM_TYPE_TIME] = mShader.getFragmentUniformLocation("uTime");
				mUniformFragmentShaderLocation[UNIFORM_TYPE_MODEL_MATRIX] = mShader.getFragmentUniformLocation("uModelMatrix");
				mUniformFragmentShaderLocation[UNIFORM_TYPE_PROJECTION_VIEW_MATRIX] = mShader.getFragmentUniformLocation("uProjectionViewMatrix");
			}
		};

		rio::Texture2D* GetTexture(TextureType textureType) { return mTextures[textureType]; };
		rio::TextureSampler2D* GetTextureSampler(TextureType textureType) { return &mTextureSamplers[textureType]; };
		rio::RenderState* GetRenderState() { return &mRenderState; };

		void SetTextureLocation(TextureType textureType, u32 vertLocation, u32 fragLocation) 
		{
			mTextureFragmentShaderLocation[textureType] = fragLocation;
			mTextureVertexShaderLocation[textureType] = vertLocation;
		};
		void SetTexture(rio::Texture2D* texture, TextureType textureType) 
		{
			if (mTextures[textureType])
				delete mTextures[textureType];

			mTextures[textureType] = texture;
			mTextureSamplers[textureType].linkTexture2D(mTextures[textureType]);
		};

		u32 GetUniformFragmentLocation(UniformType uniformType) { return mUniformFragmentShaderLocation[uniformType]; };
		u32 GetUniformVertexLocation(UniformType uniformType) { return mUniformVertexShaderLocation[uniformType]; };

		void SetRoughness(double roughness) { mRoughness = roughness; }
		void SetMetallic(double metallic) { mMetallic = metallic; }

		const std::string& GetName() const { return mName; };
		void SetName(std::string name) { mName = name; };

		// Move constructor
		Material(Material&& other) noexcept = default;

		// Move assignment operator
		Material& operator=(Material&& other) noexcept = default;

		// Copy constructor
		Material(const Material& other) = default;

		// Copy assignment operator
		Material& operator=(const Material& other) = default;
		
	private:
		std::string mName;

		rio::Texture2D* mTextures[TEXTURE_TYPE_MAX];
		rio::TextureSampler2D mTextureSamplers[TEXTURE_TYPE_MAX];

		u32 mTextureFragmentShaderLocation[TEXTURE_TYPE_MAX];
		u32 mTextureVertexShaderLocation[TEXTURE_TYPE_MAX];

		rio::Shader mShader;
		rio::Shader::ShaderMode mShaderMode = rio::Shader::ShaderMode::MODE_UNIFORM_REGISTER;

		rio::RenderState mRenderState;

		double mMetallic = 0.0;
		double mRoughness = 0.0;

		u32 mUniformFragmentShaderLocation[UNIFORM_TYPE_MAX];
		u32 mUniformVertexShaderLocation[UNIFORM_TYPE_MAX];
	};
}