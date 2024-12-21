#include "Scenes/TestScene.h"
#include "Properties/CharacterController.hpp"

#include "gfx/rio_Window.h"

#include "rio-e/Systems/ModelLoader.h"
#include "rio-e/Common/FFL.h"

double mCounter;
void TestScene::Start()
{
	mMiiCharacter = CreateNode("Mii");
	mMiiCharacter->SetPosition({ 0, 500, 0 });
	mMiiCharacter->AddProperty(std::make_shared<CharacterController>());
	mMiiCharacter->AddChild(rioe::ModelLoader::LoadModel("Banana.glb", "Player")->GetRootNode());
	GetCamera()->pos().set(0, 10, 20);

	rioe::ModelLoader::LoadModel("Angel.glb", "angel")->GetRootNode()->SetScale({ 20, 20, 20 });

	{
		rioe::FFL::InitializeFFL();
		rioe::FFL::InitializeRandomDatabase(32);

		mMiiShader = new MiiShader();
		mMiiShader->Init();

		mFFLCharModelSource.dataSource = FFL_DATA_SOURCE_STORE_DATA;
		mFFLCharModelSource.index = 0;

		FFLStoreData mii = rioe::FFL::LoadStoreData("Mii/NoNameGave.ffsd");
		mFFLCharModelSource.pBuffer = &mii;

		mFFLCharModelDesc.modelFlag = 1 << 0 | 1 << 1 | 1 << 2;
		mFFLCharModelDesc.resourceType = FFL_RESOURCE_TYPE_MIDDLE;
		mFFLCharModelDesc.resolution = FFLResolution(1028);
		mFFLCharModelDesc.expressionFlag = 1;

		FFLSetExpression(&mFFLCharModel, FFL_EXPRESSION_WINK_RIGHT);

		FFLResult result = FFLInitCharModelCPUStep(&mFFLCharModel, &mFFLCharModelSource, &mFFLCharModelDesc);

		mMiiShader->Bind();
		FFLInitCharModelGPUStep(&mFFLCharModel);

		RIO_LOG("Result: %d\n", (s32)result);
	}
}

void TestScene::UpdateStep()
{
	
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
////#if RIO_IS_WIN
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
////#else
	//VPADStatus status;
	//VPADReadError error;
  //
	//VPADRead(VPAD_CHAN_0, &status, 1, &error);
  //
	//ImGui_ImplWiiU_ControllerInput input;
  //
	//input.vpad = &status;
  //
	//ImGui_ImplWiiU_ProcessInput(&input);
  //
	//ImGui_ImplGX2_NewFrame();
////#endif
////
////	ImGui::NewFrame();
////
////	ImGui::Begin("Debugger");
////
////	ImGui::BeginChild("nodes", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 2 }, true);
////
////	for (auto& node : *rioe::GetEngine()->GetActiveScene()->GetAllNodes())
	//{
	//	if (node.second->GetParent())
	//		continue;
  //
	//	RecursiveDebug(node.second);
	//}
////
////	ImGui::EndChild();
////
////	ImGui::BeginChild("stats", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y }, true);
////
////	{
	//	rioe::Input::InputInfo& inp = rioe::Input::GetInputInfo();
  //
	//	ImGui::SeparatorText("Input");
	//	ImGui::Text("Left Stick: %f %f", inp.lStickDir.x, inp.lStickDir.y);
	//	ImGui::Text("Right Stick: %f %f", inp.rStickDir.x, inp.rStickDir.y);
	//	ImGui::Text("Cursor X: %d", inp.curX);
	//	ImGui::Text("Cursor Y: %d", inp.curY);
	//	ImGui::Text("Is touching: %d", (int)inp.touched);
	//}
////	
////	{
	//	rioe::Time::DateTime date = *rioe::Time::GetDateTime();
	//	f32 time = rioe::Time::GetTime();
  //
	//	ImGui::SeparatorText("Time");
	//	ImGui::Text("Current DateTime: %d-%d-%d %d:%d:%d", date.year, date.month, date.day, date.hours, date.minutes, date.seconds);
	//	ImGui::Text("Current Time: %f\n", time);
	//}
////
////	{
	//	ImGui::SeparatorText("Engine");
  //
	//	ImGui::Text("Current Scene Name: %s", rioe::GetEngine()->GetActiveSceneName().c_str());
	//	ImGui::Text("Registered Scene Count: %d", rioe::GetEngine()->GetSceneList().size());
	//}
////	
////	ImGui::EndChild();
////
////	ImGui::End();
////
////	ImGui::EndFrame();
////	ImGui::Render();
////
////#if RIO_IS_WIN
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
////#else
	//ImGui_ImplGX2_RenderDrawData(ImGui::GetDrawData());
////#endif

	mMiiShader->Bind();
	rio::BaseMtx34f viewMtx;
	
	GetCamera()->getMatrix(&viewMtx);
	
	rio::Matrix44f mat = mMiiCharacter->GetWorldMatrix();
	mat.applyScaleLocal({ 0.5f, 0.5f, 0.5f });
	
	rio::Matrix44f pers;
	
	rio::MemUtil::copy(&pers, &GetPerspectiveProjection().getMatrix(), sizeof(rio::Matrix44f));
	
	mMiiShader->setViewUniform(mat, viewMtx, pers);
	
	rio::RenderState opa_render_state;
	opa_render_state.setDepthEnable(true, true);
	opa_render_state.setDepthFunc(rio::Graphics::COMPARE_FUNC_LEQUAL);
	opa_render_state.applyDepthAndStencilTest();
	mMiiShader->applyAlphaTest(true, rio::Graphics::COMPARE_FUNC_GREATER, 0.0f);
	opa_render_state.setBlendEnable(false);
	opa_render_state.setBlendFactor(rio::Graphics::BLEND_MODE_ONE, rio::Graphics::BLEND_MODE_ZERO);
	opa_render_state.applyBlendAndFastZ();
	
	FFLDrawOpa(&mFFLCharModel);
	
	rio::RenderState xlu_render_state;
	xlu_render_state.setDepthEnable(true, false);
	xlu_render_state.setDepthFunc(rio::Graphics::COMPARE_FUNC_LEQUAL);
	xlu_render_state.applyDepthAndStencilTest();
	mMiiShader->applyAlphaTest(true, rio::Graphics::COMPARE_FUNC_GREATER, 0.0f);
	xlu_render_state.setBlendEnable(true);
	xlu_render_state.setBlendFactor(rio::Graphics::BLEND_MODE_SRC_ALPHA, rio::Graphics::BLEND_MODE_ONE_MINUS_SRC_ALPHA);
	xlu_render_state.applyBlendAndFastZ();
	
	FFLDrawXlu(&mFFLCharModel);
}

void TestScene::Exit()
{
	FFLDeleteCharModel(&mFFLCharModel);

	rioe::FFL::Cleanup();

	delete mMiiShader;
}