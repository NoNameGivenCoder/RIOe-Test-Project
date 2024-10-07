#pragma once

#include "rio-e/Types/Node.h"
#include "rio-e/Types/Material.h"

#include "gpu/rio_VertexArray.h"
#include "gpu/rio_VertexBuffer.h"
#include "gpu/rio_VertexStream.h"

#include <unordered_map>

namespace rioe
{
	class SkeletalMesh
	{
	public:
		struct Vertex
		{
			rio::Vector3f pos;
			rio::Vector3f normal;
			rio::Vector2f tex_coord;
			rio::Vector4f weights;
			rio::Vector4u joints;
		};

		SkeletalMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indicies);
		~SkeletalMesh() { delete mMaterial; }

		void Draw() const;
		Material* GetMaterial() { return mMaterial; };

		SkeletalMesh(SkeletalMesh&& other) noexcept = default;

		// Move assignment operator
		SkeletalMesh& operator=(SkeletalMesh&& other) noexcept = default;

		// Copy constructor
		SkeletalMesh(const SkeletalMesh& other) = default;

		// Copy assignment operator
		SkeletalMesh& operator=(const SkeletalMesh& other) = default;

	private:
		friend class ModelLoader;
		std::vector<Vertex> mVertices;
		std::vector<unsigned int> mIndicies;

		rio::VertexBuffer mVBO;
		rio::VertexArray mVAO;

		rio::VertexStream mPosStream;
		rio::VertexStream mTexCoordStream;
		rio::VertexStream mNormalStream;
		rio::VertexStream mWeightStream;
		rio::VertexStream mJointStream;

		const unsigned int* mIdxBuf;
		size_t mIdxNum;

		Material* mMaterial;
	};

	class Skeleton
	{
	public:
		std::vector<SkeletalMesh*>& GetMeshes() {
			return mMeshes;
		};

		std::vector<std::shared_ptr<Node>>& GetBones() {
			return mBones;
		};
	private:
		friend class ModelLoader;
		std::vector<SkeletalMesh*> mMeshes;
		std::vector<std::shared_ptr<Node>> mBones;
	};
}