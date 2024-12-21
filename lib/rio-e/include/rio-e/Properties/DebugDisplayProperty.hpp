#pragma once

#include "rio-e/Types/Property.h"
#include "rio-e/Types/Node.h"
#include "rio-e/Types/Skeleton.h"

#include "gfx/rio_PrimitiveRenderer.h"

#include "misc/rio_MemUtil.h"

#include <vector>

namespace rioe {
	namespace properties {
		class DebugDisplayProperty : public IProperty
		{
		public:
			~DebugDisplayProperty() { };

			//void Start() override
			//{
			//
			//}
			//
			//void UpdateStep() override
			//{
			//	auto parentNode = GetParentNode().lock().get();
			//
			//	if (!parentNode)
			//		return;
			//
			//	rio::Matrix44f worldMatrix = parentNode->GetWorldMatrix();
			//
			//	rio::PrimitiveRenderer::instance()->drawSphere8x16({ worldMatrix.m[0][3], worldMatrix.m[1][3], worldMatrix.m[2][3] }, 0.1, mColor);
			//}

			void ChangeColor(rio::Color4f color) { mColor = color; };


			rioe::Skeleton* mSkeleton;
			int boneId;

		private:
			rio::Color4f mColor = rio::Color4f::cRed;
		};
	}
}