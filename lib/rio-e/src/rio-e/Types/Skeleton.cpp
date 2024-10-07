#include "rio-e/Types/Skeleton.h"
#include "gpu/rio_Drawer.h"

namespace rioe
{
	SkeletalMesh::SkeletalMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indicies) : mMaterial(nullptr)
	{
		mVertices = vertices;
		mIndicies = indicies;

		mIdxBuf = mIndicies.data();
		mIdxNum = mIndicies.size();

		rio::VertexBuffer::invalidateCache(mIdxBuf, mIdxNum * sizeof(unsigned int));

		mVBO.setStride(sizeof(rioe::SkeletalMesh::Vertex));
		mVBO.setDataInvalidate(mVertices.data(), mVertices.size() * sizeof(rioe::SkeletalMesh::Vertex));

		mPosStream.setLayout(0, rio::VertexStream::FORMAT_32_32_32_FLOAT, offsetof(rioe::SkeletalMesh::Vertex, pos));
		mNormalStream.setLayout(1, rio::VertexStream::FORMAT_32_32_32_FLOAT, offsetof(rioe::SkeletalMesh::Vertex, normal));
		mTexCoordStream.setLayout(2, rio::VertexStream::FORMAT_32_32_FLOAT, offsetof(rioe::SkeletalMesh::Vertex, tex_coord));
		mWeightStream.setLayout(3, rio::VertexStream::FORMAT_32_32_32_32_FLOAT, offsetof(rioe::SkeletalMesh::Vertex, weights));
		mJointStream.setLayout(4, rio::VertexStream::FORMAT_32_32_32_32_UINT, offsetof(rioe::SkeletalMesh::Vertex, joints));

		mVAO.initialize();

		mVAO.addAttribute(mPosStream, mVBO);
		mVAO.addAttribute(mNormalStream, mVBO);
		mVAO.addAttribute(mTexCoordStream, mVBO);
		mVAO.addAttribute(mWeightStream, mVBO);
		mVAO.addAttribute(mJointStream, mVBO);

		mVAO.process();
	}

	void SkeletalMesh::Draw() const
	{
		mVAO.bind();
		rio::Drawer::DrawElements(rio::Drawer::TRIANGLES, mIdxNum, mIdxBuf);
	}
}