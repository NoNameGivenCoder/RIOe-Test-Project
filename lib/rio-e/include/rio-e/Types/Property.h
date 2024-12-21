#pragma once

#include <memory>

namespace rioe
{
    class Node;
    class Engine;
    class IProperty
    {
    public:
        IProperty() { Start(); };
        virtual ~IProperty() = default;

    public:
        // Runs at the start of the scene, or when the property is created
        virtual void Start() {};
        // Runs at every logic step
        virtual void UpdateStep() {};
        // Runs at every draw step
        virtual void DrawStep() {};
        // Stores the property name in string format
        virtual const char* GetPropertyName() { return "Property"; };

        inline std::weak_ptr<Node> GetParentNode() const { return parentNode; };
    private:
        friend class Node;
        std::weak_ptr<Node> parentNode;
    };
}