#include "rio-e/Types/Scene.h"
#include <gpu/rio_Shader.h>

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

    void IScene::SetEnvironmentShaderInfo(rio::Shader* shader, u32 sunColorFragLocation, u32 sunDirectionFragLocation)
    {
        shader->setUniform(rio::Vector4f({ mEnvInfo.SunColor.r, mEnvInfo.SunColor.g, mEnvInfo.SunColor.b, mEnvInfo.SunColor.a }), u32(-1), sunColorFragLocation);
        shader->setUniform(mEnvInfo.SunDirection, u32(-1), sunDirectionFragLocation);
    }
}