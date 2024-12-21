#include "rio-e/Engine.hpp"
#include "nn/ffl.h"
#include "MiiShader.h"

class TestScene : public rioe::IScene
{
public:
	TestScene() {};

	void Start() override;
	void UpdateStep() override;
	void LateUpdateStep() override;
	void DrawStep() override;
	void Exit() override;
	void LateDrawStep() override;

private:
	std::shared_ptr<rioe::Node> mMiiCharacter;

	FFLCharModel mFFLCharModel;
	FFLCharModelDesc mFFLCharModelDesc;
	FFLCharModelSource mFFLCharModelSource;

	MiiShader* mMiiShader;
};