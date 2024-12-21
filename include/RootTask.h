#include <task/rio_Task.h>
#include "rio-e/Types/Node.h"
#include "rio-e/Types/Skeleton.h"

#include "rio-e/Types/UI/UIElement.h"

#include "rio-e/Types/Scene.h"
#include "rio-e/Types/Model.h"

class RootTask : public rio::ITask
{
public:
    RootTask();

private:
    void prepare_() override;
    void calc_() override;
    void exit_() override;

private:
    struct SkeletalBlock
    {
        rio::Matrix44f projection_matrix;
        rio::Matrix44f modelMtx;
        u8 _padding[128];
    };

    struct SkeletalBoneBlock
    {
        rio::Matrix44f boneMatrices[40];
    };

    double mCounter = 0.0;

    std::shared_ptr<rioe::Node> mNode;
    rioe::Skeleton* mSkeleton;

    rio::UniformBlock* uniformSkeletalBoneBlock;
    SkeletalBoneBlock* skeletalBoneBlockArray;

    rio::UniformBlock* uniformSkeletalBlock;
    SkeletalBlock* skeletalBlockArray;

    rio::UniformBlock* uniformEnvironmentBlock;
    rioe::Model::EnviromentBlock* environmentBlockArray;

    rio::Matrix44f mProjMtx;
    rio::Matrix44f mModelMtx = rio::Matrix44f::ident;
    rioe::UIElement* mUIElement;

    rioe::EnvironmentInfo* mEnvInfo;
};