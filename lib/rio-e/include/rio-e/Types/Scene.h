#pragma once

#include "rio-e/Types/Node.h"

#include "gfx/rio_Projection.h"
#include "gfx/rio_Camera.h"

#include "gfx/rio_PrimitiveRenderer.h"

#include <unordered_map>
#include <memory>

namespace rioe
{
    class Scene
    {
    public:
        inline std::shared_ptr<Node> GetNodeByID(int ID) { return mNodes[ID]; };
        inline const std::unordered_map<int, std::shared_ptr<Node>> GetAllNodes() { return mNodes; };
        inline const size_t GetNodeCount() const { return mNodes.size(); };

        inline const std::string& GetSceneName() const { return sceneName; };

        inline rio::LookAtCamera* GetCamera() { return &mCamera; };
        inline rio::PerspectiveProjection* GetPerspectiveProjection() { return &mProjection; };

        inline void DeleteNode(int ID) 
        { 
            auto node = GetNodeByID(ID);
            mNodes.erase(ID); 
            
            if (auto parent = node->GetParent())
            {
                auto& children = parent->GetChildrenMutable();
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
        inline std::shared_ptr<Node> CreateNode()
        {
            auto node = std::make_shared<Node>();
            node->ID = mNodes.size() + 1;
            node->name = "Node (" + std::to_string(node->ID) + ")";

            mNodes.try_emplace(node->ID, node);

            return node;
        }

        inline void Update()
        {
            rio::PrimitiveRenderer::instance()->setCamera(mCamera);
            rio::PrimitiveRenderer::instance()->setProjection(mProjection);

            for (const auto& node : mNodes)
            {
                for (const auto& property : node.second->mProperties)
                {
                    property->Update();
                }
            }
        }
    private:
        friend class SceneMgr;

        std::unordered_map<int, std::shared_ptr<Node>> mNodes;
        std::string sceneName;

        rio::LookAtCamera mCamera;
        rio::PerspectiveProjection mProjection;

        std::string mLog;
    };
}