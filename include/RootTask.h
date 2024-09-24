#include <task/rio_Task.h>
#include "rio-e/Types/Mesh.h"
#include "gpu/rio_Shader.h"
#include "math/rio_Matrix.h"
#include "gpu/rio_UniformBlock.h"

class RootTask : public rio::ITask
{
public:
    RootTask();

private:
    void prepare_() override;
    void calc_() override;
    void exit_() override;

private:
    rioe::Mesh* mMesh;
    rio::Shader mShader;
    double mCounter = 0.0;

    rio::UniformBlock* mViewUniformBlock;
    rio::UniformBlock* mModelUniformBlock;
    rio::Matrix44f mProjMtx;

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

    ModelBlock mModelBlock;
    ViewBlock mViewBlock;
};