#include <rio-e/Types/Property.h>
#include <rio-e/Types/Node.h>
#include <rio-e/Common/Input.h>

#include <math/rio_Vector.h>

class CharacterController : public rioe::IProperty
{
public:
	void UpdateStep()
	{
		auto controller = rioe::Input::GetController(mControllerIdx);

		//Gravity
		auto parentNode = GetParentNode().lock();
		auto parentPos = parentNode->GetPosition();

		if (parentPos.y <= 0)
			mIsGrounded = true;
		else
			mIsGrounded = false;

		if (mIsGrounded)
		{
			mVelocity.y = 0;
		}
		else
		{
			mVelocity.y -= mGravity;
		}

		float dampingFactor = 0.9f;
		mVelocity.x *= dampingFactor;
		mVelocity.z *= dampingFactor;

		const float epsilon = 0.01f;
		if (std::abs(mVelocity.x) < epsilon) mVelocity.x = 0.0f;
		if (std::abs(mVelocity.z) < epsilon) mVelocity.z = 0.0f;
		
		MovementStep(controller);
		CameraMovementStep(controller);

		parentNode->SetPosition(parentPos + mVelocity);
	};

private:
	void CameraMovementStep(rioe::Input::ControllerInputInfo& controller)
	{
		static float yaw = 0.0f;
		static float pitch = 0.5f;

		yaw += controller.rStickDir.x * mCameraSensitivity;
		pitch += controller.rStickDir.y * mCameraSensitivity;

		float maxPitch = rio::Mathf::deg2rad(89.0f);
		pitch = std::clamp(pitch, -maxPitch, maxPitch);

		auto parentNode = GetParentNode().lock();

		float camX = parentNode->GetPosition().x + mCameraRadius * cos(pitch) * cos(yaw);
		float camY = parentNode->GetPosition().y + mCameraRadius * sin(pitch);
		float camZ = parentNode->GetPosition().z + mCameraRadius * cos(pitch) * sin(yaw);

		auto camera = rioe::GetEngine()->GetActiveScene()->GetCamera();

		camera->pos().set(camX, camY, camZ);
		camera->at() = parentNode->GetPosition();
	}

	void MovementStep(rioe::Input::ControllerInputInfo& controller)
	{
		rio::Vector2f lStick = controller.lStickDir;

		auto parentNode = GetParentNode().lock();
		auto parentPos = parentNode->GetPosition();

		auto camera = rioe::GetEngine()->GetActiveScene()->GetCamera();

		rio::Vector3f cameraPos = camera->pos();
		rio::Vector3f cameraAt = camera->at();
		rio::Vector3f cameraUp = rio::Vector3f({ 0, 1, 0 });

		rio::Vector3f cameraForward = cameraAt - cameraPos;
		cameraForward.y = 0.0f;
		cameraForward.normalize();

		rio::Vector3f cameraRight = cameraForward.cross(cameraUp);
		cameraRight.normalize();

		rio::Vector3f movementDirection = cameraRight * lStick.x + cameraForward * lStick.y;

		movementDirection *= mCharacterSpeed;

		mVelocity.x += movementDirection.x;
		mVelocity.z += movementDirection.z;

		if (mIsGrounded && controller.buttonsPressed.isOn(1 << rioe::Input::BUTTON_A))
		{
			mVelocity.y += mCharacterJumpHeight;
		}
	}
private:
	rioe::Input::ControllerIndex mControllerIdx = rioe::Input::CONTROLLER_0;

	float mCameraRadius = 100.f;
	float mCameraSensitivity = 0.05f;
	float mCharacterSpeed = 1.f;
	float mCharacterJumpHeight = 15.f;

	float mGravity = 0.4f;

	bool mIsGrounded = false;

	rio::Vector3f mVelocity = { 0, 0, 0 };
};