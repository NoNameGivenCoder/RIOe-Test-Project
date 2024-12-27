#pragma once

#include "vpad/input.h"

namespace rioe
{
	inline void Input::Calc()
	{
		// Gamepad
		{
			VPADStatus status;
			VPADReadError error;

			VPADRead(VPAD_CHAN_0, &status, 1, &error);

			VPADTouchData touchInfo;
			VPADGetTPCalibratedPoint(VPAD_CHAN_0, &touchInfo, &status.tpFiltered2);

			ControllerInputInfo* inpInfo = &mCurrentInputInfo.controllers[CONTROLLER_0];

			inpInfo->controllerType = CONTROLLER_TYPE_GAMEPAD;

			inpInfo->touched = (bool)status.tpNormal.touched;
			inpInfo->buttonsPressed.makeAllZero();

			inpInfo->curX = (u16)(touchInfo.x);
			inpInfo->curY = (u16)(touchInfo.y);

			inpInfo->lStickDir = { status.leftStick.x, status.leftStick.y };
			inpInfo->rStickDir = { status.rightStick.x, status.rightStick.y };

			inpInfo->buttonsPressed.change(1 << BUTTON_A, status.trigger & VPAD_BUTTON_A);
			inpInfo->buttonsPressed.change(1 << BUTTON_B, status.trigger & VPAD_BUTTON_B);
			inpInfo->buttonsPressed.change(1 << BUTTON_X, status.trigger & VPAD_BUTTON_X);
			inpInfo->buttonsPressed.change(1 << BUTTON_Y, status.trigger & VPAD_BUTTON_Y);

			inpInfo->buttonsPressed.change(1 << BUTTON_LEFT, status.trigger & VPAD_BUTTON_L);
			inpInfo->buttonsPressed.change(1 << BUTTON_RIGHT, status.trigger & VPAD_BUTTON_R);

			inpInfo->buttonsPressed.change(1 << BUTTON_START, status.trigger & VPAD_BUTTON_PLUS);
			inpInfo->buttonsPressed.change(1 << BUTTON_SELECT, status.trigger & VPAD_BUTTON_MINUS);
			inpInfo->buttonsPressed.change(1 << BUTTON_HOME, status.trigger & VPAD_BUTTON_HOME);
		}
		
	}
}