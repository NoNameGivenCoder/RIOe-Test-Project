#pragma once

#include <memory>

namespace rioe
{
    class Node;
    class Property
    {
    public:
        Property() {};
        virtual ~Property() = default;

    public:
        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void CreatePropertiesMenu() = 0;

        virtual YAML::Node Save() = 0;
        virtual void Load(YAML::Node node) = 0;

    public:
        inline std::weak_ptr<Node> GetParentNode() const { return parentNode; };
    private:
        friend class Node;
        std::weak_ptr<Node> parentNode;
    };
}