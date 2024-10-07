#pragma once

#include "rio-e/Types/Property.h"
#include "rio-e/Types/Node.h"

#include "gfx/rio_PrimitiveRenderer.h"

#include "misc/rio_MemUtil.h"

#include <vector>

namespace rioe {
	namespace properties {
		class DebugDisplayProperty : public Property
		{
		public:
			~DebugDisplayProperty() { };

			void Start() override
			{

			}

			void Update() override
			{
				auto parentNode = GetParentNode().lock().get();

				if (!parentNode)
					return;

				rio::Matrix34f worldMatrix;
				parentNode->GetWorldMatrix(&worldMatrix);

				rio::PrimitiveRenderer::instance()->begin();
				rio::PrimitiveRenderer::instance()->drawSphere8x16({ worldMatrix.m[0][3], worldMatrix.m[1][3], worldMatrix.m[2][3] }, 0.1, mColor);
				rio::PrimitiveRenderer::instance()->end();
			}

			void ChangeColor(rio::Color4f color) { mColor = color; };

			void Load(YAML::Node node) override {};
			void CreatePropertiesMenu() override {};
			YAML::Node Save() override { return YAML::Node(); };

		private:
			rio::Color4f mColor = rio::Color4f::cRed;
		};
	}
}