#pragma once

#include "rio.h"
#include "math/rio_Math.h"
#include "math/rio_Matrix.h"
#include "math/rio_Vector.h"

#include "rio-e/Types/Property.h"

#include <string>
#include <memory>
#include <vector>

namespace rioe
{
    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        int ID;
        std::string name;

        inline rio::Vector3f GetScale() const { return mScale; };
        inline rio::Vector3f GetPosition() const { return mPosition; };
        inline rio::Vector3f GetRotation() const { return mRotation; };

        inline std::shared_ptr<Node> GetParent() const { return mParent.lock(); };

        inline void AddChild(std::shared_ptr<Node> child)
        {
            if (IsDescendantOf(child))
            {
                RIO_LOG("[Node] Cannot parent node to a descendant!\n");
                return;
            }

            if (child->GetParent())
                child->GetParent()->RemoveChild(child);

            mChildren.push_back(child); 
            child->mParent = shared_from_this(); 
        }
        void RemoveChild(std::shared_ptr<Node> child)
        {
            auto it = std::remove(mChildren.begin(), mChildren.end(), child);
            if (it != mChildren.end())
            {
                mChildren.erase(it, mChildren.end());
                child->mParent.reset();
            }
        }

        inline std::vector<std::shared_ptr<Node>>& GetChildrenMutable() { return mChildren; };
        inline const std::vector<std::shared_ptr<Node>>& GetChildren() const { return mChildren; };

        inline void SetScale(rio::Vector3f pScale)
        {
            mScale = pScale;
            UpdateMatrix();
        };
        inline void SetPosition(rio::Vector3f pPos)
        {
           mPosition = pPos;
           UpdateMatrix();
        };
        inline void SetRotation(rio::Vector3f pRot)
        {
            mRotation = pRot;
            UpdateMatrix();
        };

        inline void AddProperty(std::shared_ptr<Property> property)
        {
            property->Start();
            mProperties.push_back(property);
        }

    private:
        rio::Matrix34f mTransformMatrix;
        rio::Vector3f mPosition;
        rio::Vector3f mRotation;
        rio::Vector3f mScale;

        std::vector<std::shared_ptr<Property>> mProperties;
        std::vector<std::shared_ptr<Node>> mChildren;
        std::weak_ptr<Node> mParent;

        friend class Node;
    private:
        // Updating the transformation matrix using the three vectors
        inline void UpdateMatrix() { mTransformMatrix.makeSRT(mScale, mRotation, mPosition); };

        // If a node is the descendant of another node.
        inline bool IsDescendantOf(std::shared_ptr<Node> node)
        {
            auto current = shared_from_this();
            while (auto parent = current->GetParent())
            {
                if (parent == node)
                {
                    return true;
                }
                current = parent;
            }
            return false;
        }
    };
}