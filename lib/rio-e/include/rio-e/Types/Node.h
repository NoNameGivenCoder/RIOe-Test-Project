#pragma once

#include "rio.h"
#include "math/rio_Math.h"
#include "math/rio_Matrix.h"
#include "math/rio_Vector.h"
#include "math/rio_Quat.h"

#include "rio-e/Types/Property.h"

#include <string>
#include <memory>
#include <vector>
#include <iterator>
#include <algorithm>

namespace rioe
{
    class Node : public std::enable_shared_from_this<Node>
    {
    public:
        int ID;
        std::string name;

        Node() : ID(-1) {};
        ~Node() { mProperties.clear(); };

        inline rio::Vector3f GetScale() const { return mScale; };
        inline rio::Vector3f GetPosition() const { return mTranslation; };
        inline rio::Quatf GetRotation() const { return mRotation; };

        inline rio::Matrix44f GetLocalMatrix() 
        {
            CalculateMatrix(rio::Matrix44f::ident);

            return mLocalTransformMatrix; 
        };
        inline rio::Matrix44f GetWorldMatrix()
        {
            rio::Matrix44f parentMatrix = rio::Matrix44f::ident;

            if (GetParent())
                parentMatrix = GetParent()->GetWorldMatrix();

            CalculateMatrix(parentMatrix);

            return mWorldTransformMatrix;
        };

        inline std::shared_ptr<Node> GetParent() const { return mParent.lock(); };

        void AddChild(std::shared_ptr<Node> child)
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

        inline std::vector<std::shared_ptr<Node>>& GetChildren() { return mChildren; };

        inline void SetScale(const rio::Vector3f& scale)
        {
            mScale = scale;
            return SetDirty();
        };
        inline void SetPosition(const rio::Vector3f& pos)
        {
            mTranslation = pos;
            return SetDirty();
        };
        inline void SetRotation(const rio::Quatf rot)
        {
            mRotation = rot;
            return SetDirty();
        }

        inline void AddProperty(std::shared_ptr<IProperty> property)
        {
            property->parentNode = shared_from_this();
            mProperties.push_back(property);
        }
        
        template <typename T>
        std::shared_ptr<T> GetProperty()
        {
            static_assert(std::is_base_of<IProperty, T>::value, "T must inherit from base class IProperty.");

            for (const auto& property : mProperties)
            {
                if (auto propertyFound = std::dynamic_pointer_cast<T>(property))
                    return propertyFound;
            }
        }

    private:
        rio::Matrix44f mLocalTransformMatrix = rio::Matrix44f::ident;
        rio::Matrix44f mWorldTransformMatrix = rio::Matrix44f::ident;
        bool mDirtyMatrix = true;

        rio::Vector3f mTranslation = { 0, 0, 0 };
        rio::Vector3f mScale = { 1, 1, 1 };
        rio::Quatf mRotation = { 0, 0, 0, 0 };

        std::vector<std::shared_ptr<IProperty>> mProperties;
        std::vector<std::shared_ptr<Node>> mChildren;
        std::weak_ptr<Node> mParent;

        friend class Engine;
    private:
        inline void SetDirty()
        {
            mDirtyMatrix = true;

            for (const auto& child : mChildren)
                child->SetDirty();
        };

        inline void CalculateMatrix(rio::Matrix44f parentMatrix)
        {
            if (mDirtyMatrix)
            {
                mLocalTransformMatrix.makeSQT(mScale, mRotation, mTranslation);
                
                mDirtyMatrix = false;
            }

            mWorldTransformMatrix.setMul(parentMatrix, mLocalTransformMatrix);

            for (const auto& child : mChildren)
            {
                child->CalculateMatrix(mWorldTransformMatrix);
            } 
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
        };
    };
}