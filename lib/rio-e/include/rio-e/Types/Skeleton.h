#pragma once

#include "rio-e/Types/Node.h"
#include "rio-e/Types/Material.h"

#include "gpu/rio_VertexArray.h"
#include "gpu/rio_VertexBuffer.h"
#include "gpu/rio_VertexStream.h"

#include <unordered_map>

namespace rioe
{
	struct Keyframe
	{
		rio::Vector3f translation = { 0, 0, 0 };
		rio::Vector3f scale = { 0, 0, 0 };
		rio::Quatf rotation = { 0, 0, 0, 0 };

		f32 time;
	};

	enum AnimationInterpolation
	{
		Linear,
		Step,
		Cubic
	};

	enum ChannelTarget
	{
		Translation,
		Rotation,
		Scale
	};

	struct AnimationChannel {
		int nodeIndex;
		AnimationInterpolation interpolation;
		ChannelTarget target;
		std::vector<Keyframe> keyframes;
	};

	struct Animation {
		std::string name;
		std::vector<AnimationChannel> channels;
	};

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
		static constexpr int MAX_BONES = 30;
	public:
		std::vector<SkeletalMesh*>& GetMeshes() {
			return mMeshes;
		};

		std::vector<std::shared_ptr<Node>>& GetBones() {
			return mBones;
		};

		std::vector<rio::Matrix44f> GetInverseBindMatrices() {
			return mInverseBindMatrices;
		};

		std::shared_ptr<rioe::Node> GetRootNode() {
			return mRootNode;
		};

		std::unordered_map<int, std::shared_ptr<Node>> GetAnimationMap()
		{
			return mBoneAnimationMap;
		}

		void ApplyAnimation(std::string animationName, f32 time);
	private:
		friend class ModelLoader;
		std::vector<SkeletalMesh*> mMeshes;
		std::vector<std::shared_ptr<Node>> mBones;
		std::unordered_map<int, std::shared_ptr<Node>> mBoneAnimationMap;
		std::vector<rio::Matrix44f> mInverseBindMatrices;

		std::unordered_map<std::string, Animation> mAnimations;

		std::shared_ptr<rioe::Node> mRootNode;
	};
}