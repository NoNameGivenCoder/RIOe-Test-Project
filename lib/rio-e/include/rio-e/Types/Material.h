#include <string>
#include <vector>

#include "gpu/rio_Texture.h"
#include "gpu/rio_Shader.h"
#include "gpu/rio_UniformBlock.h"
#include "gpu/rio_RenderState.h"

namespace rioe
{
	class Texture
	{

	};

	class Material
	{
	public:
		void Bind() const;

		const rio::Shader* GetShader() const { return mShader; };
		const std::string& GetName() const { return mName; };
		const std::vector<Texture>& GetTextures() const { return mTextures; };
	private:
		friend class MeshDisplayProperty;
		std::string mName;
		std::vector<std::string> mTexNames;
		std::string mShaderNames;

		std::vector<rio::UniformBlock> mUniformBlocks;

		std::vector<Texture> mTextures;
		rio::Shader* mShader;

		rio::RenderState mRenderState;
	};
}