#include "Scenes/TestScene.h"
#include "Properties/CharacterController.hpp"

#include "gfx/rio_Window.h"

#include "rio-e/Systems/ModelLoader.h"
#include "rio-e/Common/FFL.h"

#include "rio-e/Properties/SkeletonDisplayProperty.hpp"
#include "rio-e/Properties/MeshDisplayProperty.hpp"

double mCounter;
void TestScene::Start()
{
	mMiiCharacter = CreateNode("Mii");
	mMiiCharacter->SetPosition({ 0, 100, 0 });
	mMiiCharacter->AddProperty(std::make_shared<CharacterController>());
	//mMiiCharacter->AddChild(rioe::ModelLoader::LoadModel("Banana.glb", "Player")->GetRootNode());

	mMiiCharacter->GetProperty<CharacterController>()->SetCharModel(&mFFLCharModel);
	GetCamera()->pos().set(0, 10, 20);
	 
	auto node = rioe::ModelLoader::LoadModel("Angel.glb", "Ground");
	//node->SetScale({ 50, 50, 50 });
	//node->SetPosition({ 0, -10, 0 });
	//node->SetRotation(rioe::Utils::QuaternionFromEulerAngles({ rio::Mathf::deg2rad(90), 0, 0 }));
	//mSkeleton = rioe::ModelLoader::LoadSkeleton("TestToad.glb", "Toad");

	//mSkeleton->GetRootNode()->SetScale({ 5, 5, 5 });
	//mSkeleton->GetRootNode()->SetPosition({ 0, 0, 0 });
	//mSkeleton->GetRootNode()->SetRotation(rioe::Utils::QuaternionFromEulerAngles(rio::Mathf::deg2rad(-90), 0, 0));
	//
	//mHeadNode = mSkeleton->GetBones()[8];

	{
		rioe::FFL::InitializeFFL();
		rioe::FFL::InitializeRandomDatabase(32);
		
		mMiiShader = new MiiShader();
		mMiiShader->Init();
		
		FFLStoreData mii = rioe::FFL::LoadStoreData("Mii/NoNameGave.ffsd");
		mFFLCharModelSource = {
			.dataSource = FFL_DATA_SOURCE_STORE_DATA,
			.pBuffer = &mii,
			.index = 0
		};
		
		// Specify the expressions being used: normal, wink right.
		const u32 expressionFlag = 1 << FFL_EXPRESSION_NORMAL
			                     | 1 << FFL_EXPRESSION_BLINK
								 | 1 << FFL_EXPRESSION_SURPRISE_OPEN_MOUTH;
		
		mFFLCharModelDesc = {
			.resolution = FFL_RESOLUTION_TEX_768,
			.expressionFlag = expressionFlag,
			.modelFlag = FFL_MODEL_FLAG_NORMAL,
			.resourceType = FFL_RESOURCE_TYPE_HIGH//MIDDLE
		};
		
		FFLResult result = FFLInitCharModelCPUStep(&mFFLCharModel, &mFFLCharModelSource, &mFFLCharModelDesc);
		
		RIO_ASSERT(result == FFL_RESULT_OK);
		
		mMiiShader->Bind();
		FFLInitCharModelGPUStep(&mFFLCharModel);
	} 

	//mMiiCharacter->GetChildren()[0]->GetChildren()[0]->GetProperty<rioe::properties::MeshDisplayProperty>()->mMeshes[0]->GetMaterial()->SetTexture(newTexture, rioe::Material::TEXTURE_TYPE_ALBEDO);
}

void TestScene::UpdateStep()
{
	mCounter += 0.2f / 60;
}

void TestScene::DrawStep()
{
	rio::Window::instance()->clearColor(0.2, 0.3, 0.3);
	rio::Window::instance()->clearDepthStencil();
}

void TestScene::LateUpdateStep()
{
	auto controller = rioe::Input::GetController(rioe::Input::CONTROLLER_0);
	
	mCounter += 0.2f / 60;
}

void TestScene::LateDrawStep()
{
	//mMiiShader->Bind();
	//rio::BaseMtx34f viewMtx;
	//
	//GetCamera()->getMatrix(&viewMtx);
	//
	//int headBoneIndex = 8;
	//rio::Matrix44f skinMatrix = mMiiCharacter->GetWorldMatrix();
	////skinMatrix.setMul(skinMatrix, mSkeleton->GetInverseBindMatrices()[8]);
	//
	//// Transform using the parent node matrix
	//rio::Matrix44f headWorldMatrix;
	//
	//rio::Matrix44f headSkinMatrix;
	////headSkinMatrix.setMul(mSkeleton->GetInverseBindMatrices()[8], mSkeleton->GetBones()[8]->GetWorldMatrix());
	////
	////headWorldMatrix.setMul(mSkeleton->GetRootNode()->GetWorldMatrix(), headSkinMatrix);
	//
	//// Apply scale (if necessary)
	//skinMatrix.applyScaleLocal({ 0.5f, 0.5f, 0.5f });
	////headWorldMatrix.applyTranslationWorld({ 0, 0, 0 });
	//
	////headWorldMatrix.applyScaleLocal({ 0.1f, 0.1f, 0.1f });
	////headWorldMatrix.applyTranslationWorld({ 0, 10, 0 });
	//
	//rio::Matrix44f pers;
	//
	//rio::MemUtil::copy(&pers, &GetPerspectiveProjection().getMatrix(), sizeof(rio::Matrix44f));
	//
	//mMiiShader->setViewUniform(skinMatrix, viewMtx, pers);
	//
	//rio::RenderState opa_render_state;
	//opa_render_state.setDepthEnable(true, true);
	//opa_render_state.setDepthFunc(rio::Graphics::COMPARE_FUNC_LEQUAL);
	//opa_render_state.applyDepthAndStencilTest();
	//mMiiShader->applyAlphaTest(true, rio::Graphics::COMPARE_FUNC_GREATER, 0.0f);
	//opa_render_state.setBlendEnable(false);
	//opa_render_state.setBlendFactor(rio::Graphics::BLEND_MODE_ONE, rio::Graphics::BLEND_MODE_ZERO);
	//opa_render_state.applyBlendAndFastZ();
	//
	//FFLDrawOpa(&mFFLCharModel);
	//
	//rio::RenderState xlu_render_state;
	//xlu_render_state.setDepthEnable(true, false);
	//xlu_render_state.setDepthFunc(rio::Graphics::COMPARE_FUNC_LEQUAL);
	//xlu_render_state.applyDepthAndStencilTest();
	//mMiiShader->applyAlphaTest(true, rio::Graphics::COMPARE_FUNC_GREATER, 0.0f);
	//xlu_render_state.setBlendEnable(true);
	//xlu_render_state.setBlendFactor(rio::Graphics::BLEND_MODE_SRC_ALPHA, rio::Graphics::BLEND_MODE_ONE_MINUS_SRC_ALPHA);
	//xlu_render_state.applyBlendAndFastZ();
	//
	//FFLDrawXlu(&mFFLCharModel);
}

void TestScene::Exit()
{
	FFLDeleteCharModel(&mFFLCharModel);
	
	rioe::FFL::Cleanup();
	
	delete mMiiShader;

	mMiiCharacter.reset();
	mHeadNode.reset();
}