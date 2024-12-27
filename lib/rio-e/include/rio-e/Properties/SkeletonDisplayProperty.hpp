#pragma once

#include "rio-e/Types/Property.h"
#include "rio-e/MeshLoader.h"

#include "rio-e/Types/Mesh.h"
#include "rio-e/Types/Material.h"
#include "rio-e/Types/Model.h"

#include "rio-e/Engine.hpp"

#include "gfx/mdl/rio_Material.h"

#include "misc/rio_MemUtil.h"

#include <vector>

namespace rioe {
	namespace properties {
		class SkeletonDisplayProperty : public IProperty
		{
		public:
			struct SkeletalBlock
			{
				rio::Matrix44f viewProjectionMatrix;
				rio::Matrix44f parentNodeMatrix;
			};

			struct SkeletalBoneBlock
			{
				rio::Matrix44f boneMatrices[rioe::Skeleton::MAX_BONES];
			};

		public:
			~SkeletonDisplayProperty()
			{
				Cleanup();
			}

			const char* GetPropertyName() override { return "SkeletonDisplay"; }

			void DrawStep() override
			{
				mSkeleton->ApplyAnimation("Test", rioe::Time::GetTime());

				for (int i = 0; i < mSkeleton->GetMeshes().size(); i++) {
					const auto& mesh = mSkeleton->GetMeshes()[i];

					mesh->GetMaterial()->Bind();

					rio::BaseMtx34f viewMtx;
					rioe::GetEngine()->GetActiveScene()->GetCamera()->getMatrix(&viewMtx);

					rio::Matrix44f viewProjectionMatrix;

					viewProjectionMatrix.setMul(mProjMtx, viewMtx);
					viewProjectionMatrix.transpose();

					mSkeletalBlockArray[i].viewProjectionMatrix = viewProjectionMatrix;
					mSkeletalBlockArray[i].parentNodeMatrix = mSkeleton->GetRootNode()->GetWorldMatrix();

					for (int s = 0; s < mSkeleton->GetBones().size(); s++)
					{
						rio::Matrix44f realMtx = mSkeleton->GetBones()[s]->GetWorldMatrix();
						realMtx.setMul(realMtx, mSkeleton->GetInverseBindMatrices()[s]);

						realMtx.transpose();

						mSkeletalBoneBlockArray[i].boneMatrices[s] = realMtx;
					}

					if (i == 0) {
						mUniformSkeletalBoneBlockArray[0].setSubDataInvalidate(&mSkeletalBoneBlockArray[0], 0, sizeof(SkeletalBoneBlock));
						mUniformSkeletalBoneBlockArray[0].bind();
					}

					mUniformSkeletalBlockArray[i].setSubDataInvalidate(&mSkeletalBlockArray[i], 0, sizeof(SkeletalBlock));
					mUniformSkeletalBlockArray[i].bind();

					mesh->Draw();
				}
			};

			void Cleanup()
			{
				if (loaded)
				{
					delete[] mUniformSkeletalBlockArray;
					delete[] mUniformSkeletalBoneBlockArray;

					delete[] mSkeletalBoneBlockArray;
					delete[] mSkeletalBlockArray;
				}
			}

			void Initialize()
			{
				rio::MemUtil::copy(&mProjMtx, &rioe::GetEngine()->GetActiveScene()->GetPerspectiveProjection().getMatrix(), sizeof(rio::Matrix44f));

				mUniformSkeletalBlockArray = new rio::UniformBlock[mSkeleton->GetMeshes().size()];
				mSkeletalBlockArray = new SkeletalBlock[mSkeleton->GetMeshes().size()];
				
				mUniformSkeletalBoneBlockArray = new rio::UniformBlock[mSkeleton->GetMeshes().size()];
				mSkeletalBoneBlockArray = new SkeletalBoneBlock[mSkeleton->GetMeshes().size()];

				for (int i = 0; i < mSkeleton->GetMeshes().size(); i++)
				{
					const auto& mesh = mSkeleton->GetMeshes()[i];
					mesh->GetMaterial()->LoadShader("skinning_demo");

					mUniformSkeletalBlockArray[i].setStage(rio::UniformBlock::STAGE_VERTEX_SHADER);
					mUniformSkeletalBlockArray[i].setIndex(mesh->GetMaterial()->GetShader()->getVertexUniformBlockIndex("SkeletalBlock"));
					mUniformSkeletalBlockArray[i].setDataInvalidate(&mSkeletalBlockArray[i], sizeof(SkeletalBlock));

					mUniformSkeletalBoneBlockArray[i].setStage(rio::UniformBlock::STAGE_VERTEX_SHADER);
					mUniformSkeletalBoneBlockArray[i].setIndex(mesh->GetMaterial()->GetShader()->getVertexUniformBlockIndex("SkeletalBoneBlock"));
					mUniformSkeletalBoneBlockArray[i].setDataInvalidate(&mSkeletalBoneBlockArray[i], sizeof(SkeletalBoneBlock));
				}
			}

			void PlayAnimation(std::string& animationName)
			{
				mLastPlayTime = rioe::Time::GetTime();
			}

			void SetSkeleton(Skeleton* skeleton)
			{
				mSkeleton = skeleton;
			}

			Skeleton* GetSkeleton()
			{
				return mSkeleton;
			}

		private:
			rio::UniformBlock* mUniformSkeletalBlockArray;
			SkeletalBlock* mSkeletalBlockArray;

			rio::UniformBlock* mUniformSkeletalBoneBlockArray;
			SkeletalBoneBlock* mSkeletalBoneBlockArray;

			bool loaded = false;

			rio::Matrix44f mProjMtx;

			Skeleton* mSkeleton;

			float mLastPlayTime = 0.0f;

			friend class ModelLoader;
		};
	}
}