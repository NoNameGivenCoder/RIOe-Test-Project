#include "Scenes/TestScene2.h"

#include "gfx/rio_Window.h"

void TestScene2::Start()
{
	
}

void TestScene2::UpdateStep()
{
	if (rioe::Input::GetController(rioe::Input::CONTROLLER_0).lStickDir.x == 1)
	{
		rioe::GetEngine()->ChangeActiveScene("TestScene");
	}
}

void TestScene2::DrawStep()
{
	rio::Window::instance()->clearColor(0.5, 0.3, 0.3);
}

void TestScene2::Exit()
{
	
}