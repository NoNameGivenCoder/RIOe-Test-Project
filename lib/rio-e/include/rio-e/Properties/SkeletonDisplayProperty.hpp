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
			~SkeletonDisplayProperty()
			{
				Cleanup();
			}

			const char* GetPropertyName() override { return "SkeletonDisplay"; }

			void DrawStep() override
			{
				
			};

			//TODO: replace with a rioe::Mesh**
			std::vector<rioe::Mesh*> mMeshes;

			void Cleanup()
			{
				if (loaded)
				{
					
				}
			}

			void Initialize()
			{
				rio::MemUtil::copy(&mProjMtx, &rioe::GetEngine()->GetActiveScene()->GetPerspectiveProjection().getMatrix(), sizeof(rio::Matrix44f));
			}

		private:
			u32* mParentNodeMatrixLocation;
			u32* mViewXProjectionMatrixLocation;

			bool loaded = false;

			rio::Matrix44f mProjMtx;

			friend class ModelLoader;
		};
	}
}