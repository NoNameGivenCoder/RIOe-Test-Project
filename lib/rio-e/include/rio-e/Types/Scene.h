#pragma once

#include "rio-e/Types/Node.h"

#include "gfx/rio_Projection.h"
#include "gfx/rio_Camera.h"
#include "gfx/rio_Color.h"

#include <unordered_map>
#include <memory>
#include <cstring>

namespace rio { class Shader; }

namespace rioe
{
    class Material;

    struct EnvironmentInfo
    {
        rio::Vector3f SunDirection = { 0, 0, 0 };

        //x r, g y, b z
        rio::Color4f SunColor = rio::Color4f::cWhite;
    };

    class IScene
    {
    public:
        IScene() 
        {
            mOrthoProjection.set(-1.0f, 1.0f, 720.0f, 0.0f, 0.0f, 1280.0f);
            mPerspectiveProjection.set(1.f, 10000.0f, rio::Mathf::deg2rad(90.0f), static_cast<f32>(1280) / 720);
        };

		~IScene() { ClearNodes(); };
    public:
        std::shared_ptr<Node> GetNodeByID(int ID) { return mNodes.at(ID); };
        std::unordered_map<int, std::shared_ptr<Node>>* GetAllNodes() { return &mNodes; };
        const size_t GetNodeCount() { return mNodes.size(); };
    public:
        rio::LookAtCamera* GetCamera() { return &mCamera; };
        rio::PerspectiveProjection& GetPerspectiveProjection() { return mPerspectiveProjection; };
        rio::OrthoProjection& GetOrthoProjection() { return mOrthoProjection; };
        EnvironmentInfo& GetEnvironmentInfo() { return mEnvInfo; };
    public:
        void SetEnvironmentInfo(EnvironmentInfo& environmentInfo) { mEnvInfo = environmentInfo; };
        void SetPerspectiveProjection(rio::PerspectiveProjection& perspectiveProjection) { mPerspectiveProjection = perspectiveProjection; };
        void SetOrthoProjection(rio::OrthoProjection& orthoProjection) { mOrthoProjection = orthoProjection; };
    public:
        // Sets uniforms for fragment shader. Shader must be binded before.
        void SetEnvironmentMaterialInfo(rioe::Material* material);
    public:
        // Deletes node with provided ID, deletes child nodes too.
        void DeleteNode(int ID);

        // Creates a new node
        std::shared_ptr<Node> CreateNode(const char* name = "");

        // Clears all nodes within a scene.
        void ClearNodes() 
        {
            mNodes.clear();
        };

    protected:
        friend class Engine;
        // Before all properties have received a Start() call
        virtual void Start() {};
        // After all properties have received a Start() call
        virtual void LateStart() {};
        // Before all properties have received a UpdateStep() call
        virtual void UpdateStep() {};
        // After all properties have received a UpdateStep() call
        virtual void LateUpdateStep() {};
        // Before all properties have received a Draw() call
        virtual void DrawStep() {};
        // After all properties have received a Draw() call
        virtual void LateDrawStep() {};
        // Called when scene is about to exit
        virtual void Exit() {};
    private:
        std::unordered_map<int, std::shared_ptr<Node>> mNodes;

        rio::LookAtCamera mCamera;
        rio::PerspectiveProjection mPerspectiveProjection;
        rio::OrthoProjection mOrthoProjection;

        EnvironmentInfo mEnvInfo;
    };
}