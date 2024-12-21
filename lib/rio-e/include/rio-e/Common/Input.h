#pragma once

#include "rio.h"
#include "math/rio_Vector.h"
#include "misc/rio_BitFlag.h"

namespace rioe
{
	class Input
	{
	public:
		enum ControllerType
		{
			CONTROLLER_TYPE_GAMEPAD = 0,
			CONTROLLER_TYPE_REMOTE,
			CONTROLLER_TYPE_KEYBOARD,
			CONTROLLER_TYPE_MAX
		};

		enum ControllerIndex
		{
			CONTROLLER_0 = 0,
			CONTROLLER_1,
			CONTROLLER_2,
			CONTROLLER_3,
			CONTROLLER_4,
			CONTROLLER_MAX
		};

		enum ButtonType
		{
			BUTTON_A = 0,
			BUTTON_B,
			BUTTON_X,
			BUTTON_Y,

			BUTTON_START,
			BUTTON_SELECT,
			BUTTON_POWER,

			BUTTON_DPAD_UP,
			BUTTON_DPAD_DOWN,
			BUTTON_DPAD_LEFT,
			BUTTON_DPAD_RIGHT,

			BUTTON_RIGHT_TRIGGER,
			BUTTON_LEFT_TRIGGER,

			BUTTON_RIGHT,
			BUTTON_LEFT,

			BUTTON_MAX
		};

		struct ControllerInputInfo
		{
			ControllerType controllerType;

			rio::Vector2f lStickDir;
			rio::Vector2f rStickDir;

			rio::BitFlag32 buttonsPressed;
			rio::BitFlag32 buttonsHeld;
			rio::BitFlag32 buttonsRelease;

			u16 curX;
			u16 curY;
			bool touched;

			u8 volume;

			// Cafe Specific
			u8 battery;
		};

		struct InputInfo
		{
			ControllerInputInfo controllers[CONTROLLER_MAX];
			u8 controllersConnected;
		};

		static void Calc();
		static ControllerInputInfo& GetController(ControllerIndex idx) { return mCurrentInputInfo.controllers[idx]; };
		static u8 GetNumControllersConnected() { return mCurrentInputInfo.controllersConnected; };
	private:
		static InputInfo mCurrentInputInfo;
	};
};

#if RIO_IS_WIN
#include "rio-e/Common/win/InputImplWin.h"
#else
#include "rio-e/Common/cafe/InputImplCafe.h"
#endif