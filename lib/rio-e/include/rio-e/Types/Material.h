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
		Material() : mTexture(nullptr), mTextureSampler(nullptr), mShader(nullptr), mTextureLocation(0) {};
		~Material() {  }
		void Bind() const;

		rio::Shader* GetShader() const { return mShader.get(); };
		rio::Texture2D* GetTexture() const { return mTexture.get(); };
		rio::TextureSampler2D* GetSamplerTexture() const { return mTextureSampler.get(); };
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

		std::shared_ptr<rio::Texture2D> mTexture;
		std::shared_ptr<rio::TextureSampler2D> mTextureSampler;
		std::shared_ptr<rio::Shader> mShader;

		rio::RenderState mRenderState;

		u32 mTextureLocation;
	};
}