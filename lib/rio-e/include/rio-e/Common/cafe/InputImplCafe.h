#pragma once

#include "vpad/input.h"

namespace rioe
{
	inline void Input::Calc()
	{
		//VPADStatus status;
		//VPADReadError error;
		//
		//VPADRead(VPAD_CHAN_0, &status, 1, &error);
		//
		//mCurrentInputInfo.touched = (bool)status.tpNormal.touched;
		//
		//VPADTouchData touchInfo;
		//
		//VPADGetTPCalibratedPoint(VPAD_CHAN_0, &touchInfo, &status.tpFiltered2);
		//
		//mCurrentInputInfo.curX = (u16)(touchInfo.x);
		//mCurrentInputInfo.curY = (u16)(touchInfo.y);
		//
		//mCurrentInputInfo.lStickDir = { status.leftStick.x, status.leftStick.y };
		//mCurrentInputInfo.rStickDir = { status.rightStick.x, status.rightStick.y };
	}
}