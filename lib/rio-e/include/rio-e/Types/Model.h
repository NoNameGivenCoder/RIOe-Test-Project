#pragma once

#include "rio-e/Types/Mesh.h"

#include "gpu/rio_UniformBlock.h"
#include "gpu/rio_Texture.h"
#include "gpu/rio_TextureSampler.h"
#include "math/rio_Matrix.h"

namespace rioe
{
    class Model
    {
    public:
        std::vector<Mesh*>& GetMeshes() {
            return mMeshes;
        };

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
    private:
        std::vector<Mesh*> mMeshes;

        friend class ModelLoader;
    };

}