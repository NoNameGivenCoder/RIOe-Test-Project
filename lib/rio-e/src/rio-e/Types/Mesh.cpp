#include "rio-e/Types/Mesh.h"
#include "gpu/rio_Drawer.h"

namespace rioe
{
	// Majority of this was copied over from rio::mdl::Mesh but changed to remove the res mesh.
	Mesh::Mesh(std::vector<rio::mdl::res::Vertex> vertices, std::vector<unsigned int> indicies)
	{
		this->vertices = vertices;
		this->indicies = indicies;

		RIO_LOG("vert %d\n", vertices.size());

		mIdxBuf = this->indicies.data();  // Now safe to access
		mIdxNum = this->indicies.size();

		rio::VertexBuffer::invalidateCache(mIdxBuf, mIdxNum);

		mVBO.setStride(sizeof(rio::mdl::res::Vertex));
		mVBO.setDataInvalidate(vertices.data(), vertices.size() * sizeof(rio::mdl::res::Vertex));

		mPosStream.setLayout(0, rio::VertexStream::FORMAT_32_32_32_FLOAT, offsetof(rio::mdl::res::Vertex, pos));
		mNormalStream.setLayout(1, rio::VertexStream::FORMAT_32_32_32_FLOAT, offsetof(rio::mdl::res::Vertex, normal));
		mTexCoordStream.setLayout(2, rio::VertexStream::FORMAT_32_32_FLOAT, offsetof(rio::mdl::res::Vertex, tex_coord));

		mVAO.initialize();

		mVAO.addAttribute(mPosStream, mVBO);
		mVAO.addAttribute(mNormalStream, mVBO);
		mVAO.addAttribute(mTexCoordStream, mVBO);
		mVAO.process();

		RIO_LOG("%d during construct\n", mIdxNum);
	}

	void Mesh::Draw() const
	{
		//RIO_LOG("%d during draw\n", mIdxNum);
		mVAO.bind();
		rio::Drawer::DrawElements(rio::Drawer::TRIANGLES, mIdxNum, mIdxBuf);
	}
}