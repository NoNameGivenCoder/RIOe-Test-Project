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
    static void LogMatrix(const rio::Matrix34f& matrix)
    {
        for (int i = 0; i < 3; ++i)
        {
            RIO_LOG("%f %f %f %f\n", matrix.m[i][0], matrix.m[i][1], matrix.m[i][2], matrix.m[i][3]);
        }
    };

    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        int ID;
        std::string name;

        inline rio::Vector3f GetScale() const { return mScale; };
        inline rio::Vector3f GetPosition() const { return mPosition; };
        inline rio::Vector3f GetRotation() const { return mRotation; };

        inline rio::Matrix34f& GetLocalMatrix() { return mTransformMatrix; };
        inline void GetWorldMatrix(rio::Matrix34f* pMatrix)
        {
            if (mParent.lock())
            {
                rio::Matrix34f parentMatrix;
                mParent.lock()->GetWorldMatrix(&parentMatrix);
                pMatrix->setMul(parentMatrix, mTransformMatrix);
            }
            else
            {
                *pMatrix = mTransformMatrix;
            }    
        };

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
            //child->UpdateMatrix();
        }
        void RemoveChild(std::shared_ptr<Node> child)
        {
            auto it = std::remove(mChildren.begin(), mChildren.end(), child);
            if (it != mChildren.end())
            {
                mChildren.erase(it, mChildren.end());
                child->mParent.reset();
                //child->UpdateMatrix();
            }
        }

        inline std::vector<std::shared_ptr<Node>>& GetChildrenMutable() { return mChildren; };
        inline const std::vector<std::shared_ptr<Node>>& GetChildren() const { return mChildren; };

        inline void SetScale(rio::Vector3f scale)
        {
            mScale = scale;
            mTransformMatrix.setScaleWorld(mScale);
            //UpdateMatrix();
        };
        inline void SetPosition(rio::Vector3f pos)
        {
            mPosition = pos;
            mTransformMatrix.setTranslationWorld(pos);
            //UpdateMatrix();
        };
        inline void SetRotation(rio::Vector3f rot)
        {
            mRotation = rot;
            mTransformMatrix.setRotationWorld(rot);
            //UpdateMatrix();
        };

        inline void AddProperty(std::shared_ptr<Property> property)
        {
            property->parentNode = shared_from_this();
            mProperties.push_back(property);
            property->Start();
        }
        
        template <typename T>
        std::vector<T*> GetProperty()
        {
            std::vector<T*> result;

            for (const auto& property : mProperties)
            {
                if (T* propertyFound = dynamic_cast<T*>(property.get()))
                {
                    result.push_back(propertyFound);
                }
            }

            return result;
        }

    private:
        rio::Matrix34f mTransformMatrix = rio::Matrix34f::ident;
        rio::Vector3f mPosition;
        rio::Vector3f mRotation;
        rio::Vector3f mScale;

        std::vector<std::shared_ptr<Property>> mProperties;
        std::vector<std::shared_ptr<Node>> mChildren;
        std::weak_ptr<Node> mParent;

        friend class Scene;
    private:
        friend class Node;

        inline void UpdateMatrix()
        {
            
        };

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