#include <task/rio_Task.h>
#include "rio-e/Types/Mesh.h"
#include "gpu/rio_Shader.h"
#include "math/rio_Matrix.h"

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


    rio::Matrix44f mProjMtx;
};