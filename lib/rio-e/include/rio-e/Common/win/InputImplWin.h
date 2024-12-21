#pragma once

#include "misc/gl/rio_GL.h"
#include "gfx/rio_Window.h"

#include <unordered_map>

namespace rioe
{
	static std::unordered_map<int, bool> keyState;
	static std::unordered_map<int, bool> keyPressed;

	inline void UpdateKeyState(GLFWwindow* window, int key)
	{
		bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
		if (isPressed && !keyState[key])
			keyPressed[key] = true;
		else
			keyPressed[key] = false;

		keyState[key] = isPressed;
	}

	inline void Input::Calc()
	{
		GLFWwindow* window = rio::Window::instance()->getNativeWindow().getGLFWwindow();
		ControllerInputInfo* inpInfo = &mCurrentInputInfo.controllers[CONTROLLER_0];

		mCurrentInputInfo.controllersConnected = 1;

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		int isTouch = glfwGetMouseButton(window, 0);

		inpInfo->curX = x;
		inpInfo->curY = y;
		inpInfo->touched = isTouch;

		inpInfo->buttonsPressed.makeAllZero();

		const char* joystickName = glfwGetJoystickName(0);

		// If no joystick, then use keyboard controls for joystick
		if (!joystickName)
		{
			float xLF = 0, yLF = 0, xRF = 0, yRF = 0;

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				yLF += 1.0f;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				yLF += -1.0f;

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				xLF += -1.0f;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				xLF += 1.0f;

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				yRF += 1.0f;
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				yRF += -1.0f;

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				xRF += -1.0f;
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				xRF += 1.0f;

			inpInfo->lStickDir = { xLF, yLF };
			inpInfo->rStickDir = { xRF, yRF };

			UpdateKeyState(window, GLFW_KEY_SPACE);
				
			inpInfo->buttonsPressed.change(1 << BUTTON_A, keyPressed[GLFW_KEY_SPACE]);
		}
		else
		{
			int cnt;
			GLFWgamepadstate gamepadState;
			glfwGetGamepadState(0, &gamepadState);

			inpInfo->lStickDir.set(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X], -gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
			inpInfo->rStickDir.set(gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], -gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);

			inpInfo->buttonsPressed.change(1 << BUTTON_A, gamepadState.buttons[GLFW_GAMEPAD_BUTTON_A]);
			inpInfo->buttonsPressed.change(1 << BUTTON_B, gamepadState.buttons[GLFW_GAMEPAD_BUTTON_B]);
			inpInfo->buttonsPressed.change(1 << BUTTON_X, gamepadState.buttons[GLFW_GAMEPAD_BUTTON_X]);
			inpInfo->buttonsPressed.change(1 << BUTTON_Y, gamepadState.buttons[GLFW_GAMEPAD_BUTTON_Y]);
		}
	}
}