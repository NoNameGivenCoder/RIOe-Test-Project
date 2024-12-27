#include <rio-e/Types/Property.h>
#include <rio-e/Types/Node.h>
#include <rio-e/Common/Input.h>

#include <math/rio_Vector.h>

#include <rio-e/Common/Utils.h>

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
			if (mCharModel)
				FFLSetExpression(mCharModel, FFL_EXPRESSION_NORMAL);

			mVelocity.y = 0;
		}
		else
		{
			if (mCharModel)
				FFLSetExpression(mCharModel, FFL_EXPRESSION_SURPRISE_OPEN_MOUTH);

			mVelocity.y -= mGravity;
		}

		float dampingFactor = 0.85f;
		mVelocity.x *= dampingFactor;
		mVelocity.z *= dampingFactor;

		const float epsilon = 0.001f;
		if (std::abs(mVelocity.x) < epsilon) mVelocity.x = 0.0f;
		if (std::abs(mVelocity.z) < epsilon) mVelocity.z = 0.0f;
		
		MovementStep(controller);
		CameraMovementStep(controller);

		parentNode->SetPosition(parentPos + mVelocity);
	};

	void SetCharModel(FFLCharModel* charModel)
	{
		mCharModel = charModel;
	}

private:
	void CameraMovementStep(rioe::Input::ControllerInputInfo& controller)
	{
		auto parentNodePos = GetParentNode().lock()->GetPosition();

		rioe::GetEngine()->GetActiveScene()->GetCamera()->pos() = parentNodePos + mCameraOffset;

		rioe::GetEngine()->GetActiveScene()->GetCamera()->at() = parentNodePos;
	}

	void MovementStep(rioe::Input::ControllerInputInfo& controller)
	{
		rio::Vector2f lStick = controller.lStickDir;

		auto parentNode = GetParentNode().lock();
		auto parentPos = parentNode->GetPosition();

		auto camera = rioe::GetEngine()->GetActiveScene()->GetCamera();

		rio::Vector3f movementDirection = {lStick.x, 0.0f, -lStick.y};

		movementDirection *= mCharacterSpeed;

		mVelocity.x += movementDirection.x;
		mVelocity.z += movementDirection.z;

		if (mIsGrounded && controller.buttonsPressed.isOn(1 << rioe::Input::BUTTON_A))
		{
			mVelocity.y += mCharacterJumpHeight;
		}

		if (movementDirection.length() > 0.0f) // Prevent rotating when there's no input
		{
			movementDirection.normalize();
			
			float yaw = std::atan2(movementDirection.x, movementDirection.z);

			rio::Quatf targetRot = rioe::Utils::QuaternionFromEulerAngles(0.0f, yaw, 0.0f);
			rio::Quatf currentRotation = parentNode->GetRotation();

			rio::Quatf newRotation;

			newRotation.setSlerp(currentRotation, targetRot, 0.1f);
			newRotation.normalize();

			parentNode->SetRotation(newRotation);
		}
	}
private:
	rioe::Input::ControllerIndex mControllerIdx = rioe::Input::CONTROLLER_0;

	rio::Vector3f mCameraOffset = { 0, 135, 100 };
	float mCameraSensitivity = 0.05f;
	float mCharacterSpeed = 0.5f;
	float mCharacterJumpHeight = 5.f;

	float mGravity = 0.4f;

	bool mIsGrounded = false;

	rio::Vector3f mVelocity = { 0, 0, 0 };

	FFLCharModel* mCharModel = nullptr;
};