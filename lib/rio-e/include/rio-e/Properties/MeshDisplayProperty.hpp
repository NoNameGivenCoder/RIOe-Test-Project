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

					rioe::GetEngine()->GetActiveScene()->SetEnvironmentMaterialInfo(mesh->GetMaterial());
					mesh->GetMaterial()->GetShader()->setUniform(GetParentNode().lock()->GetWorldMatrix(), mParentNodeMatrixLocation[i], u32(-1));

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
				}
			}

			void Initialize()
			{
				if (loaded)
				{
					delete[] mParentNodeMatrixLocation;
				}

				mParentNodeMatrixLocation = new u32[mMeshes.size()];

				loaded = true;

				for (int i = 0; i < mMeshes.size(); ++i)
				{
					auto& mesh = mMeshes[i];

					if (mesh->GetMaterial()->GetShader()->isLoaded())
						mesh->GetMaterial()->GetShader()->unload();

					mesh->GetMaterial()->LoadShader("gltf_test");

					mParentNodeMatrixLocation[i] = mesh->GetMaterial()->GetShader()->getVertexUniformLocation("parentNodeMtx");
				}
			}

		private:
			u32* mParentNodeMatrixLocation;

			bool loaded = false;

			friend class ModelLoader;
		};
	}
}