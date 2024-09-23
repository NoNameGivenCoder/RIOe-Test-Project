#pragma once

#include <string>
#include <vector>

#include <gpu/rio_VertexArray.h>
#include <gfx/mdl/res/rio_MeshData.h>

namespace rioe
{
	class Mesh
	{
	public:
		Mesh(std::vector<rio::mdl::res::Vertex> vertices, std::vector<unsigned int> indicies);

		void Draw() const;

	private:
		std::vector<rio::mdl::res::Vertex> vertices;
		std::vector<unsigned int> indicies;

		rio::VertexBuffer mVBO;
		rio::VertexArray mVAO;
		rio::VertexStream mPosStream;
		rio::VertexStream mTexCoordStream;
		rio::VertexStream mNormalStream;

		const unsigned int* mIdxBuf;
		size_t mIdxNum;
	};
}