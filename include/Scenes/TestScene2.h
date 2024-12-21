#include "rio-e/Engine.hpp"

class TestScene2 : public rioe::IScene
{
public:
	TestScene2() {};

	void Start() override;
	void UpdateStep() override;
	void DrawStep() override;
	void Exit() override;
};