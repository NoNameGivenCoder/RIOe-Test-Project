#include <misc/rio_Types.h>

#if RIO_IS_WIN && defined(_WIN32)

#include <controller/win/rio_KeyboardMouseDeviceWin.h>
#include <gfx/rio_Window.h>

#include <misc/gl/rio_GL.h>
#include <misc/win/rio_Windows.h>

namespace rio {

KeyboardMouseDevice::KeyboardMouseDevice(ControllerMgr* mgr)
    : ControlDevice(mgr)
    , mIsLastReadSuccess(false)
{
    mId = ControllerDefine::DEVICE_KEYBOARD_MOUSE;
}

KeyboardMouseDevice::~KeyboardMouseDevice()
{
}

void KeyboardMouseDevice::calc()
{
    mIsLastReadSuccess = GetKeyboardState(mKeyState);

    f64 pos_x, pos_y;
    glfwGetCursorPos(Window::getWindowInner(), &pos_x, &pos_y);
    mCursorPos.set(pos_x, pos_y);
}

}

#elif RIO_IS_WIN // Non-Windows platforms

#include <controller/win/rio_KeyboardMouseDeviceWin.h>
#include <gfx/rio_Window.h>
// Include other necessary headers for non-Windows platforms

namespace rio {

KeyboardMouseDevice::KeyboardMouseDevice(ControllerMgr* mgr)
    : ControlDevice(mgr)
    , mIsLastReadSuccess(false)
{
    mId = ControllerDefine::DEVICE_KEYBOARD_MOUSE;
}

KeyboardMouseDevice::~KeyboardMouseDevice()
{
}

void KeyboardMouseDevice::calc()
{
    // Implement non-Windows-specific functionality to update keyboard and mouse state
    // For example, using SDL or another cross-platform library to get input state

    // Placeholder for cursor position retrieval (adjust as per actual implementation details)
    f64 pos_x = 0, pos_y = 0;
    // Replace glfwGetCursorPos with a non-Windows equivalent if necessary
    // glfwGetCursorPos(Window::getWindowInner(), &pos_x, &pos_y);
    mCursorPos.set(pos_x, pos_y);
    mIsLastReadSuccess = true; // Set based on actual input retrieval success
}

} // namespace rio


#endif // RIO_IS_WIN
