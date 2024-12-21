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
		class MeshDisplayProperty : public IProperty
		{
		public:
			~MeshDisplayProperty()
			{
				Cleanup();
			}

			const char* GetPropertyName() override { return "MeshDisplay"; }

			void DrawStep() override
			{
				for (int i = 0; i < mMeshes.size(); ++i)
				{
					auto& mesh = mMeshes[i];
					auto shader = mesh->GetMaterial()->GetShader();

					mesh->GetMaterial()->Bind();

					rio::Matrix44f view_proj_mtx;
					rio::BaseMtx34f viewMtx;
					rioe::GetEngine()->GetActiveScene()->GetCamera()->getMatrix(&viewMtx);

					view_proj_mtx.setMul(mProjMtx, viewMtx);

					shader->setUniform(view_proj_mtx, mViewXProjectionMatrixLocation[i], u32(-1));
					shader->setUniform(GetParentNode().lock()->GetWorldMatrix(), mParentNodeMatrixLocation[i], u32(-1));

					mesh->Draw();
				}
			};

			//TODO: replace with a rioe::Mesh**
			std::vector<rioe::Mesh*> mMeshes;

			void Cleanup()
			{
				if (loaded)
				{
					delete[] mParentNodeMatrixLocation;
					delete[] mViewXProjectionMatrixLocation;
					delete[] mSunPosition;
					delete[] mSunColor;
				}
			}

			void Initialize()
			{
				rio::MemUtil::copy(&mProjMtx, &rioe::GetEngine()->GetActiveScene()->GetPerspectiveProjection().getMatrix(), sizeof(rio::Matrix44f));

				if (loaded)
				{
					delete[] mParentNodeMatrixLocation;
					delete[] mViewXProjectionMatrixLocation;
				}

				mParentNodeMatrixLocation = new u32[mMeshes.size()];
				mViewXProjectionMatrixLocation = new u32[mMeshes.size()];
				mSunPosition = new u32[mMeshes.size()];
				mSunColor = new u32[mMeshes.size()];

				loaded = true;

				for (int i = 0; i < mMeshes.size(); ++i)
				{
					auto& mesh = mMeshes[i];

					if (mesh->GetMaterial()->GetShader()->isLoaded())
						mesh->GetMaterial()->GetShader()->unload();

					mesh->GetMaterial()->LoadShader("gltf_test");

					mParentNodeMatrixLocation[i] = mesh->GetMaterial()->GetShader()->getVertexUniformLocation("parentNodeMtx");
					mViewXProjectionMatrixLocation[i] = mesh->GetMaterial()->GetShader()->getVertexUniformLocation("viewProj");

					//mSunPosition[i] = mesh->GetMaterial()->GetShader()->getFragmentUniformLocation("sunPosition");
					//mSunColor[i] = mesh->GetMaterial()->GetShader()->getFragmentUniformLocation("sunColor");
				}
			}

		private:
			u32* mParentNodeMatrixLocation;
			u32* mViewXProjectionMatrixLocation;
			u32* mSunPosition;
			u32* mSunColor;

			bool loaded = false;

			rio::Matrix44f mProjMtx;

			friend class ModelLoader;
		};
	}
}