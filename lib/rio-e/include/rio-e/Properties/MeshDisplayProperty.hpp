#pragma once

#include "rio-e/Types/Property.h"
#include "rio-e/MeshLoader.h"

#include "rio-e/Types/Mesh.h"
#include "rio-e/Types/Material.h"

#include "gfx/mdl/rio_Material.h"

#include "rio-e/Editor/GLTFHelper.h"

#include <vector>

namespace rioe {
	namespace properties {
		class MeshDisplayProperty : public Property
		{
		public:
			~MeshDisplayProperty() override = default;

			void Start() override
			{
				//mMesh = GLTFRead("C:/Users/joeyb/Documents/Visual Studio 2022/repos/rio-e/lib/tinygltf/models/Cube/Cube.gltf");
				//mMaterial->shader()->load("test");
			}

			void Update() override
			{
				if (!mMesh)
				{

				}
				mMaterial->bind();
				mMesh->Draw();
			}

			void Load(YAML::Node node) override {};
			void CreatePropertiesMenu() override {};
			YAML::Node Save() override { return YAML::Node(); };

		private:
			Mesh* mMesh = nullptr;
			rio::mdl::Material* mMaterial;
		};
	}
}