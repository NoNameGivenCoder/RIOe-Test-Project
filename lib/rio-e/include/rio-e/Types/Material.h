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
		Material() : mTexture(nullptr), mTextureLocation(0) {};
		~Material() { delete mTexture; mShader.unload(); }
		void Bind() const;

		rio::Shader* GetShader() { return &mShader; };
		rio::Texture2D* GetTexture() const { return mTexture; };
		rio::TextureSampler2D* GetSamplerTexture() { return &mTextureSampler; };
		void SetTextureLocation(u32 pLoc) { mTextureLocation = pLoc; };

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

		rio::Texture2D* mTexture;
		rio::TextureSampler2D mTextureSampler;
		rio::Shader mShader;

		rio::RenderState mRenderState;

		u32 mTextureLocation;
	};
}