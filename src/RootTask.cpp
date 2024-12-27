#include "RootTask.h"

#include "rio-e/Systems/SceneMgr.h"

#include <gfx/rio_Window.h>
#include "rio-e/Systems/ModelLoader.h"

#include "rio-e/Types/Model.h"

#include "rio-e/Properties/ModelDisplayProperty.hpp"
#include "rio-e/Properties/DebugDisplayProperty.hpp"
//#include "rio-e/Common/Input.h"
//#include "rio-e/Common/Time.h"

#include "imgui.h"
#if RIO_IS_WIN
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#else
#include "backends/wiiu/imgui_impl_wiiu.h"
#include "backends/wiiu/imgui_impl_gx2.h"
#endif


RootTask::RootTask()
    : ITask("RootTask")
{
}

float x, y, z;

void RootTask::prepare_()
{   
    //rioe::PropertyCreatorMgr::instance()->RegisterProperty("MeshProperty", []() -> std::unique_ptr<rioe::Property> {
    //    return std::make_unique<rioe::properties::ModelDisplayProperty>();
    //});
    //
    //rioe::PropertyCreatorMgr::instance()->RegisterProperty("DebugProperty", []() -> std::unique_ptr<rioe::Property> {
    //    return std::make_unique<rioe::properties::DebugDisplayProperty>();
    //});
    //
    //rioe::SceneMgr::instance()->Load({ "DefaultScene.yaml", true });
    //mEnvInfo = rioe::SceneMgr::instance()->GetCurrentScene()->GetEnvironmentInfo();
    //
    ////mSkeleton = rioe::ModelLoader::LoadSkeleton("TestToad.glb");
    ////rioe::ModelLoader::LoadModel("MayorOffice.glb", "MayoralOffice");
    //
    ////rioe::ModelLoader::LoadModel("GhostPresentBox.glb", "GhostBox");
    //mNode = rioe::ModelLoader::LoadModel("Angel.glb", "AngelIsland")->GetRootNode();
    //
    //rio::MemUtil::copy(&mProjMtx, &rioe::SceneMgr::instance()->GetCurrentScene()->GetPerspectiveProjection()->getMatrix(), sizeof(rio::Matrix44f));
    //
    //uniformSkeletalBlock = new rio::UniformBlock[mSkeleton->GetMeshes().size()];
    //skeletalBlockArray = new SkeletalBlock[mSkeleton->GetMeshes().size()];
    //
    //uniformSkeletalBoneBlock = new rio::UniformBlock[mSkeleton->GetMeshes().size()];
    //skeletalBoneBlockArray = new SkeletalBoneBlock[mSkeleton->GetMeshes().size()];
    //
    //uniformEnvironmentBlock = new rio::UniformBlock[mSkeleton->GetMeshes().size()];
    //environmentBlockArray = new rioe::Model::EnviromentBlock[mSkeleton->GetMeshes().size()];
    //
    //alignas(256) SkeletalBlock skeletalBlockArray[40];
    //alignas(256) SkeletalBoneBlock skeletalBoneBlockArray[40];
    //alignas(256) rioe::Model::EnviromentBlock environmentBlockArray[40];
    //
    //for (int i = 0; i < mSkeleton->GetMeshes().size(); i++)
    //{
    //    const auto& mesh = mSkeleton->GetMeshes()[i];
    //    mesh->GetMaterial()->GetShader()->load("skinning_demo");
    //    
    //    uniformSkeletalBlock[i].setStage(rio::UniformBlock::STAGE_VERTEX_SHADER);
    //    uniformSkeletalBlock[i].setIndex(mesh->GetMaterial()->GetShader()->getVertexUniformBlockIndex("SkeletalBlock"));
    //    uniformSkeletalBlock[i].setDataInvalidate(&skeletalBlockArray[i], sizeof(SkeletalBlock));
    //
    //    uniformSkeletalBoneBlock[i].setStage(rio::UniformBlock::STAGE_VERTEX_SHADER);
    //    uniformSkeletalBoneBlock[i].setIndex(mesh->GetMaterial()->GetShader()->getVertexUniformBlockIndex("SkeletalBoneBlock"));
    //    uniformSkeletalBoneBlock[i].setDataInvalidate(&skeletalBoneBlockArray[i], sizeof(SkeletalBoneBlock));
    //
    //    uniformEnvironmentBlock[i].setStage(rio::UniformBlock::STAGE_FRAGMENT_SHADER);
    //    uniformEnvironmentBlock[i].setIndex(mesh->GetMaterial()->GetShader()->getFragmentUniformBlockIndex("EnvironmentBlock"));
    //    uniformEnvironmentBlock[i].setDataInvalidate(&environmentBlockArray[i], sizeof(rioe::Model::EnviromentBlock));
    //
    //    mesh->GetMaterial()->SetTextureLocation(mesh->GetMaterial()->GetShader()->getFragmentSamplerLocation("texture0"));
    //}
    //
    //mNode = rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(0);
    //
    ////rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(0)->SetScale({ 1, 1, 1 });
    //rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(0)->SetScale({ 5, 5, 5 });
    //rioe::SceneMgr::instance()->GetCurrentScene()->GetNodeByID(1)->SetRotation(rot);
    //auto property = mNode->GetProperty<rioe::properties::ModelDisplayProperty>()[0];
    //
    ////property->SetDisplayModel(rioe::ModelLoader::LoadModel("TestRig.glb"));
    ////auto property = rioe::PropertyCreatorMgr::instance()->CreateProperty("MeshProperty");
    //
    //mUIElement = new rioe::UIElement();
    //mUIElement->SetTexture((char*)"BlackFade", true);
    //mUIElement->SetRotation({ 0, 0, 0 });
    //mUIElement->SetPosition({ 0, 0 });
    //mUIElement->SetScale({ 1280, 720, 0 });
    //mUIElement->SetTransparency(1 );
    //
    //mCounter = 1.f;
    //
    //rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->pos().set(
    //    50,
    //    50,
    //    20
    //);
    //
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGui::StyleColorsDark();
    //ImGuiIO& io = ImGui::GetIO();

#if RIO_IS_WIN
    ImGui_ImplGlfw_InitForOpenGL(rio::Window::instance()->getNativeWindow().getGLFWwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
#else
   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
   //ImGui::GetStyle().ScaleAllSizes(1.5f);
   //
   //ImGui_ImplWiiU_Init();
   //ImGui_ImplGX2_Init();
#endif

    //io.DisplaySize = { 1280.f, 720.f };
}

//void RecursiveDebug(std::shared_ptr<rioe::Node> node)
//{
//    bool is_open = ImGui::TreeNodeEx(node->name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
//
//    if (is_open)
//    {
//        for (const auto& child : node->GetChildren())
//        {
//            RecursiveDebug(child);
//        }
//
//        ImGui::TreePop();
//    }
//}

void RootTask::calc_()
{
    //rio::Window::instance()->clearColor(0.2f, 0.3f, 0.3f);
    //rio::Window::instance()->clearDepthStencil();
    //
    //mCounter -= 0.01f;
    //
    ////mNode->SetPosition({0, static_cast<f32>(mCounter), 0});
    //RIO_LOG("%f\n", mCounter);
    //rio::Quatf rot = { mCounter, mCounter, mCounter, mCounter };
    //mNode->SetRotation(rot);
    //rioe::SceneMgr::instance()->GetCurrentScene()->Update();
    //mSkeleton->ApplyAnimation("Test", mCounter);
    //mUIElement->SetTransparency(mCounter);
    //
    //// Set the uniform array for each mesh
    //for (int i = 0; i < mSkeleton->GetMeshes().size(); i++) {
    //    const auto& mesh = mSkeleton->GetMeshes()[i];
    //
    //    mesh->GetMaterial()->Bind();
    //
    //    rio::Matrix44f view_proj_mtx;
    //    rio::BaseMtx34f viewMtx;
    //    rioe::SceneMgr::instance()->GetCurrentScene()->GetCamera()->getMatrix(&viewMtx);
    //
    //    view_proj_mtx.setMul(mProjMtx, viewMtx);
    //
    //    view_proj_mtx.transpose();
    //
    //    skeletalBlockArray[i].projection_matrix = view_proj_mtx;
    //    skeletalBlockArray[i].modelMtx = mModelMtx;
    //
    //    environmentBlockArray[i].light_color = mEnvInfo->SunColor;
    //    environmentBlockArray[i].light_pos = mEnvInfo->SunDirection;
    //    
    //    for (int s = 0; s < mSkeleton->GetBones().size(); s++)
    //    {
    //        rio::Matrix44f realMtx = mSkeleton->GetBones()[s]->GetWorldMatrix();
    //        realMtx.setMul(realMtx, mSkeleton->GetInverseBindMatrices()[s]);
    //
    //        realMtx.transpose();
    //
    //        skeletalBoneBlockArray[i].boneMatrices[s] = realMtx;
    //    }
    //
    //    uniformSkeletalBlock[i].setSubDataInvalidate(&skeletalBlockArray[i], 0, sizeof(SkeletalBlock));
    //    uniformSkeletalBlock[i].bind();
    //
    //    uniformSkeletalBoneBlock[i].setSubDataInvalidate(&skeletalBoneBlockArray[i], 0, sizeof(SkeletalBoneBlock));
    //    uniformSkeletalBoneBlock[i].bind();
    //
    //    uniformEnvironmentBlock[i].setSubDataInvalidate(&environmentBlockArray[i], 0, sizeof(rioe::Model::EnviromentBlock));
    //    uniformEnvironmentBlock[i].bind();
    //
    //    mesh->Draw();
    //}
    //
    //if (mCounter > 5)
    //    mCounter = 0;
    //
    ////rioe::SceneMgr::instance()->GetCurrentScene()->Update();
    //
    //rioe::Input::Calc();
    //rioe::Time::Calc();

    //RIO_LOG("%f, %f\n", inp.curX, inp.curY);

    //mUIElement->Draw();

    // Debugging
#if RIO_IS_WIN
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
#else
    VPADStatus status;
    VPADReadError error;

    VPADRead(VPAD_CHAN_0, &status, 1, &error);

    ImGui_ImplWiiU_ControllerInput input;

    input.vpad = &status;

    ImGui_ImplWiiU_ProcessInput(&input);

    ImGui_ImplGX2_NewFrame();
#endif
    //ImGui::NewFrame();
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //ImGui::End();
    //
    //ImGui::Render();

#if RIO_IS_WIN
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
    ImGui_ImplGX2_RenderDrawData(ImGui::GetDrawData());
#endif
}

void RootTask::exit_()
{
    //delete[] uniformSkeletalBlock;
    //delete[] uniformSkeletalBoneBlock;
    //
    //delete[] skeletalBoneBlockArray;
    //delete[] skeletalBlockArray;
    //
    //delete[] environmentBlockArray;
    //delete[] uniformEnvironmentBlock;

    delete mUIElement;

#if RIO_IS_WIN
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
#else
    ImGui_ImplGX2_Shutdown();
    ImGui_ImplWiiU_Shutdown();
#endif

    ImGui::DestroyContext();
}