#include "rio-e/Types/Skeleton.h"
#include "gpu/rio_Drawer.h"

namespace rioe
{
	SkeletalMesh::SkeletalMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indicies)
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

	static rio::Vector3f LerpVector(const rio::Vector3f& start, const rio::Vector3f& end, f32 factor)
	{
		return start * (1.0f - factor) + end * factor;
	}

	static Keyframe LinearInterpolateKeyframes(const Keyframe& start, const Keyframe& end, f32 time, f32 startTime, f32 endTime)
	{
		f32 factor = (time - startTime) / (endTime - startTime);

		Keyframe result;
		result.translation = LerpVector(start.translation, end.translation, factor);
		result.scale = LerpVector(start.scale, end.scale, factor);
		result.rotation.setSlerp(start.rotation, end.rotation, factor);
		result.time = time;

		return result;
	}

	Keyframe StepInterpolateKeyframes(const std::vector<Keyframe>& keyframes, f32 time)
	{
		// Step interpolation: find the keyframe where time <= the current time
		Keyframe result;

		// If there are no keyframes, return a default keyframe
		if (keyframes.empty())
		{
			return result;
		}

		// Iterate through keyframes to find the closest one that is less than or equal to the current time
		for (size_t i = 0; i < keyframes.size(); ++i)
		{
			if (keyframes[i].time > time)
			{
				// We found the first keyframe where time is greater than the current time,
				// so the previous keyframe is the one we want (step interpolation).
				return i == 0 ? keyframes[i] : keyframes[i - 1];
			}
		}

		// If time is beyond the last keyframe, return the last keyframe
		return keyframes.back();
	}

	void Skeleton::ApplyAnimation(std::string animationName, f32 time)
	{
        // Check if the animation exists
        auto it = mAnimations.find(animationName);
        if (it == mAnimations.end())
        {
            RIO_LOG("[Skeleton] Animation not found: %s\n", animationName.c_str());
            return;
        }

        // Get the animation
		Animation animation = it->second;

        // Iterate over each channel (bone) in the animation
		for (const auto& channel : animation.channels)
		{
			int boneIndex = channel.nodeIndex;

			// Find the two keyframes to interpolate between based on time
			const std::vector<Keyframe>& keyframes = channel.keyframes;

			Keyframe startKeyframe, endKeyframe;
			bool found = false;
			for (size_t i = 0; i < keyframes.size() - 1; ++i)
			{
				if (keyframes[i].time <= time && keyframes[i + 1].time >= time)
				{
					startKeyframe = keyframes[i];
					endKeyframe = keyframes[i + 1];
					found = true;
					break;
				}
			}

			if (!found)
			{
				// Handle the case where time is out of bounds (e.g., loop or clamp)
				continue;
			}

			// Interpolate between the two keyframes
			Keyframe interpolatedKeyframe;

			if (channel.interpolation == Step)
				interpolatedKeyframe = StepInterpolateKeyframes(keyframes, time);
			else
				interpolatedKeyframe = LinearInterpolateKeyframes(startKeyframe, endKeyframe, time, startKeyframe.time, endKeyframe.time);
		
			// Apply the interpolated keyframe to the bone
			// Note: mBones[boneIndex] is a shared_ptr<Node> that stores the bone's transform
			if (channel.target == Rotation)
				mBoneAnimationMap[boneIndex]->SetRotation(interpolatedKeyframe.rotation);
			else if (channel.target == Scale)
				mBoneAnimationMap[boneIndex]->SetScale(interpolatedKeyframe.scale);
			else if (channel.target == Translation)
				mBoneAnimationMap[boneIndex]->SetPosition(interpolatedKeyframe.translation);
			
			//RIO_LOG("%f %f %f %f\n", interpolatedKeyframe.rotation.x, interpolatedKeyframe.rotation.y, interpolatedKeyframe.rotation.z, interpolatedKeyframe.rotation.w);
		}

        //RIO_LOG("[Skeleton] Applied animation: %s at time %f\n", animationName.c_str(), time);
    }
}