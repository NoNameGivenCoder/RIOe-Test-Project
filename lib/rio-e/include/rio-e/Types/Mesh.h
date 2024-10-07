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
		Mesh(const std::vector<rio::mdl::res::Vertex>& vertices, const std::vector<unsigned int>& indicies);
		~Mesh() { delete mMaterial; }

		void Draw() const;
		Material* GetMaterial() { return mMaterial; };

		Mesh(Mesh&& other) noexcept = default;

		// Move assignment operator
		Mesh& operator=(Mesh&& other) noexcept = default;

		// Copy constructor
		Mesh(const Mesh& other) = default;

		// Copy assignment operator
		Mesh& operator=(const Mesh& other) = default;

	private:
		friend class ModelLoader;
		std::vector<rio::mdl::res::Vertex> mVertices;
		std::vector<unsigned int> mIndicies;

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