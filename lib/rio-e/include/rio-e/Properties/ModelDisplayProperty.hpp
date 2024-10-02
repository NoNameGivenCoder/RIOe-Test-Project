#pragma once

#include "rio-e/Types/Property.h"
#include "rio-e/MeshLoader.h"

#include "rio-e/Types/Mesh.h"
#include "rio-e/Types/Material.h"
#include "rio-e/Types/Model.h"

#include "gfx/mdl/rio_Material.h"

#include <vector>

namespace rioe {
	namespace properties {
		class ModelDisplayProperty : public Property
		{
		public:
			~ModelDisplayProperty() { Cleanup(); };

			void Start() override
			{
				
			}

			void Update() override
			{
				for (int i = 0; i < mMeshCount; i++)
				{
					auto& mesh = mDisplayModel->GetMeshes()[i];
					mesh->GetMaterial()->Bind();

					{
						rio::Matrix34f worldMtx = GetParentNode().lock()->GetTransformationMatrix();
					
						rio::Matrix34f normalMtx;
					
						normalMtx.setInverseTranspose(worldMtx);
					
						mesh->GetMaterial()->GetShader()->setUniformArray(3, worldMtx.v, mModelMatrixLocation[i], u32(-1));
						mesh->GetMaterial()->GetShader()->setUniformArray(3, normalMtx.v, mNormalMatrixLocation[i], u32(-1));
					}
					
					{
						rio::Matrix44f view_proj_mtx;
						rio::BaseMtx34f viewMtx;
						rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->getMatrix(&viewMtx);
					
						view_proj_mtx.setMul(mProjMtx, viewMtx);

						mesh->GetMaterial()->GetShader()->setUniformArray(4, view_proj_mtx.v, mViewProjectionLocation[i], u32(-1));
					}
			
					mesh->Draw();

					rio::RenderState renderState;
					renderState.apply();
				}
				
			}

			void Load(YAML::Node node) override {};
			void CreatePropertiesMenu() override {};
			YAML::Node Save() override { return YAML::Node(); };

			void SetDisplayModel(rioe::Model* pDisplayModel) { Cleanup(); mDisplayModel = pDisplayModel; InitializeModel(); };

		private:
			rioe::Model* mDisplayModel;

			u32 mMeshCount;
			u32* mModelMatrixLocation;
			u32* mNormalMatrixLocation;
			u32* mViewProjectionLocation;

			rio::Matrix44f mProjMtx;

			void Cleanup()
			{
				for (int i = 0; i < mMeshCount; i++)
				{
					if (mDisplayModel->GetMeshes()[i]->GetMaterial()->GetShader()->isLoaded())
						mDisplayModel->GetMeshes()[i]->GetMaterial()->GetShader()->unload();
				}

				delete[] mModelMatrixLocation;
				delete[] mNormalMatrixLocation;
				delete[] mViewProjectionLocation;
			}

			void InitializeModel()
			{
				rio::MemUtil::copy(&mProjMtx, &rioe::SceneMgr::instance()->GetCurrentScene()->GetPerspectiveProjection()->getMatrix(), sizeof(rio::Matrix44f));

				mMeshCount = mDisplayModel->GetMeshes().size();

				mModelMatrixLocation = new u32[mMeshCount];
				mNormalMatrixLocation = new u32[mMeshCount];
				mViewProjectionLocation = new u32[mMeshCount];

				for (int i = 0; i < mMeshCount; i++)
				{
					auto& mesh = mDisplayModel->GetMeshes()[i];

					if (mesh->GetMaterial()->GetShader()->isLoaded())
						mesh->GetMaterial()->GetShader()->unload();

					mDisplayModel->GetMeshes()[i]->GetMaterial()->GetShader()->load("gltf_test");

					mModelMatrixLocation[i] = mesh->GetMaterial()->GetShader()->getFragmentUniformLocation("model");
					mNormalMatrixLocation[i] = mesh->GetMaterial()->GetShader()->getFragmentUniformLocation("normalMtx");
					mViewProjectionLocation[i] = mesh->GetMaterial()->GetShader()->getFragmentUniformLocation("viewProj");

					mesh->GetMaterial()->SetTextureLocation(mesh->GetMaterial()->GetShader()->getFragmentSamplerLocation("texture0"));
				}
			}
		};
	}
}