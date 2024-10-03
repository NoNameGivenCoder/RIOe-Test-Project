#include "rio-e/Types/Mesh.h"
#include "gpu/rio_Drawer.h"

namespace rioe
{
	// Majority of this was copied over from rio::mdl::Mesh but changed to remove the res mesh.
	Mesh::Mesh(const std::vector<rio::mdl::res::Vertex>& vertices, const std::vector<unsigned int>& indicies) : mMaterial(nullptr)
	{
		mVertices = vertices;
		mIndicies = indicies;

		mIdxBuf = mIndicies.data();
		mIdxNum = mIndicies.size();

		rio::VertexBuffer::invalidateCache(mIdxBuf, mIdxNum * sizeof(unsigned int));

		mVBO.setStride(sizeof(rio::mdl::res::Vertex));
		mVBO.setDataInvalidate(mVertices.data(), mVertices.size() * sizeof(rio::mdl::res::Vertex));

		mPosStream.setLayout(0, rio::VertexStream::FORMAT_32_32_32_FLOAT, offsetof(rio::mdl::res::Vertex, pos));
		mNormalStream.setLayout(1, rio::VertexStream::FORMAT_32_32_32_FLOAT, offsetof(rio::mdl::res::Vertex, normal));
		mTexCoordStream.setLayout(2, rio::VertexStream::FORMAT_32_32_FLOAT, offsetof(rio::mdl::res::Vertex, tex_coord));

		mVAO.initialize();

		mVAO.addAttribute(mPosStream, mVBO);
		mVAO.addAttribute(mNormalStream, mVBO);
		mVAO.addAttribute(mTexCoordStream, mVBO);
		mVAO.process();
	}

	void Mesh::Draw() const
	{
		mVAO.bind();
		rio::Drawer::DrawElements(rio::Drawer::TRIANGLES, mIdxNum, mIdxBuf);
	}
}