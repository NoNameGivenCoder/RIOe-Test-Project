#include "rio-e/Types/Scene.h"
#include "rio-e/Types/Material.h"

namespace rioe
{
    std::shared_ptr<Node> IScene::CreateNode(const char* name)
	{
        auto node = std::make_shared<Node>();
        node->ID = mNodes.size();

        if (strlen(name) == 0)
            node->name = "Node (" + std::to_string(node->ID) + ")";
        else
            node->name = name;

        mNodes.try_emplace(node->ID, node);

        return node;
	}

    void IScene::DeleteNode(int ID)
    {
        auto node = GetNodeByID(ID);
        mNodes.erase(ID);

        if (auto parent = node->GetParent())
        {
            auto& children = parent->GetChildren();
            children.erase(std::remove(children.begin(), children.end(), node), children.end());
        }

        if (auto children = node->GetChildren(); children.size() >= 1)
        {
            for (const auto& child : children)
            {
                DeleteNode(child->ID);
            }
        }
    }
 
    void IScene::SetEnvironmentMaterialInfo(rioe::Material* material)
    {
        if (material->GetShaderMode() != rio::Shader::ShaderMode::MODE_UNIFORM_REGISTER)
        {
			RIO_LOG("[RIO(e)] Material shader mode is not set to MODE_UNIFORM_REGISTER. Cannot set environment material info.\n");
            return;
        }
			
        u32 projectionViewFragmentLocation = material->GetUniformFragmentLocation(rioe::Material::UNIFORM_TYPE_PROJECTION_VIEW_MATRIX);
        u32 projectionViewVertexLocation = material->GetUniformVertexLocation(rioe::Material::UNIFORM_TYPE_PROJECTION_VIEW_MATRIX);

        rio::Matrix34f viewMatrix;
		GetCamera()->getMatrix(&viewMatrix);

        rio::Matrix44f projectionViewMatrix;
        rio::Matrix44f mPerspectiveProjectionConverted;

        mPerspectiveProjectionConverted.v[0] = mPerspectiveProjection.getMatrix().v[0];
        mPerspectiveProjectionConverted.v[1] = mPerspectiveProjection.getMatrix().v[1];
        mPerspectiveProjectionConverted.v[2] = mPerspectiveProjection.getMatrix().v[2];
        mPerspectiveProjectionConverted.v[3] = mPerspectiveProjection.getMatrix().v[3];

		projectionViewMatrix.setMul((rio::Matrix44f)mPerspectiveProjectionConverted, viewMatrix);

		material->GetShader()->setUniform(projectionViewMatrix, projectionViewVertexLocation, projectionViewFragmentLocation);
        //shader->setUniform(rio::Vector4f({ mEnvInfo.SunColor.r, mEnvInfo.SunColor.g, mEnvInfo.SunColor.b, mEnvInfo.SunColor.a }), u32(-1), sunColorFragLocation);
        //shader->setUniform(mEnvInfo.SunDirection, u32(-1), sunDirectionFragLocation);
    }
}