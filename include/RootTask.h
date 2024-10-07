#include <task/rio_Task.h>
#include "rio-e/Types/Node.h"
#include "rio-e/Types/Skeleton.h"

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
        rio::Matrix44f root_position = rio::Matrix44f::ident;
        rio::Matrix44f boneMatrices[40];
    };

    double mCounter = 0.0;

    std::shared_ptr<rioe::Node> mNode;
    rioe::Skeleton* mSkeleton;

    rio::UniformBlock* uniformBlockArray;
    SkeletalBlock* skeletalBlockArray;

    rio::Matrix44f mProjMtx;
};