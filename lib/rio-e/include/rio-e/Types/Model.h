#pragma once

#include "rio-e/Types/Mesh.h"
#include "rio-e/Types/Node.h"

#include "gpu/rio_UniformBlock.h"
#include "gpu/rio_Texture.h"
#include "gpu/rio_TextureSampler.h"
#include "math/rio_Matrix.h"

namespace rioe
{
    class Model
    {
    public:
        std::shared_ptr<Node> GetRootNode()
        {
            return mRootNode;
        }

        struct ModelBlock
        {
            rio::Matrix34f model_mtx;
            rio::Matrix34f normal_mtx;
            rio::BaseVec4u _padding[10];
        };
        struct ViewBlock
        {
            rio::Vector3f  view_pos;       u32 _padding;
            rio::Matrix44f view_proj_mtx;
        };
        struct alignas(256) EnviromentBlock
        {
            rio::Vector3f light_color;
            rio::Vector3f light_pos;
            u8 padding[232];
        };

    private:
        std::vector<Mesh*> mMeshes;
        std::shared_ptr<Node> mRootNode;

        friend class ModelLoader;
    };

}