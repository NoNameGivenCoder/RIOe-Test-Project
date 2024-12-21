#include "gpu/rio_Drawer.h"
#include "gpu/rio_Shader.h"
#include "gpu/rio_VertexArray.h"
#include "misc/rio_MemUtil.h"
#include "gpu/rio_Texture.h"
#include "gpu/rio_TextureSampler.h"

#include "rio-e/Systems/SceneMgr.h"

#include <vector>

namespace rioe
{
	class UIElement
	{
	public:
		UIElement();
		~UIElement()
		{
			if (isTextureLoaded)
				delete mTexture;

			mShader.unload();
		}

		void Draw();
		void SetShader(char* shaderName);
		void SetTexture(char* textureName, bool scaleWithSize);

		// 0 being invisible, 1 being visible.
		void SetTransparency(f32 transparency) { mTransparency = transparency; }

		void SetPosition(rio::Vector2f pos)
		{
			mScreenPosition = pos;
			mDirtyTransform = true;
		}
		void SetScale(rio::Vector3f scale)
		{
			mScreenScale = scale;
			mDirtyTransform = true;
		}
		void SetRotation(rio::Vector3f rot)
		{
			mScreenRotation = rot;
			mDirtyTransform = true;
		}

		rio::Vector2f GetPosition() const { return mScreenPosition; };
		rio::Vector3f GetScale() const { return mScreenScale; };

		struct UIVertex
		{
			rio::BaseVec3f pos;
			rio::BaseVec2f tex_coord;
		};
	private:
		rio::VertexBuffer mVBO;
		rio::VertexArray mVAO;
		rio::VertexStream mPosStream;
		rio::VertexStream mTexCoordStream;

		std::vector<UIVertex> mVertices;

		rio::Shader mShader;
		// Vertex shader locations
		u32 mTransformMatrixLocation;
		u32 mProjectionMatrixLocation;

		// Fragment shader locations
		u32 mTransparencyLocation;

		f32 mTransparency = 1;

		bool mDirtyTransform;
		rio::Matrix44f mTransformMatrix = rio::Matrix44f::ident;
		rio::Vector2f mScreenPosition = { 0, 0 };
		rio::Vector3f mScreenScale = { 100, 100, 1 };
		rio::Vector3f mScreenRotation = { 0, 0, 0 };

		rio::Matrix44f mProjMtx;

		bool isTextureLoaded = false;
		rio::Texture2D *mTexture;
		rio::TextureSampler2D mTextureSampler;
		u32 mTextureLocation;
	};

	struct UIKeyframe
	{
		rio::BaseVec3f translation;
		rio::BaseVec3f scale;
		rio::BaseVec3f rotation;

		f32 time;
	};

	struct UIAnimation
	{
		// time, keyframe
		std::unordered_map<f32, UIKeyframe> mKeyframes;
	};
}