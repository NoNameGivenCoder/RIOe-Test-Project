#pragma once

#include <string>
#include <vector>

#include <gpu/rio_VertexArray.h>
#include <gfx/mdl/res/rio_MeshData.h>

#include "gpu/rio_Texture.h"
#include "gpu/rio_TextureSampler.h"

#include "rio-e/Types/Material.h"

namespace rioe
{
	class Mesh
	{
	public:
		Mesh(std::vector<rio::mdl::res::Vertex> vertices, std::vector<unsigned int> indicies);

		void Draw() const;
		Material* GetMaterial() { return mMaterial; };

	private:
		friend class ModelLoader;
		std::vector<rio::mdl::res::Vertex> vertices;
		std::vector<unsigned int> indicies;

		rio::VertexBuffer mVBO;
		rio::VertexArray mVAO;
		rio::VertexStream mPosStream;
		rio::VertexStream mTexCoordStream;
		rio::VertexStream mNormalStream;

		const unsigned int* mIdxBuf;
		size_t mIdxNum;

		Material* mMaterial;
	};
}