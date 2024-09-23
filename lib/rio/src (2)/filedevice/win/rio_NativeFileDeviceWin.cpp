#include <misc/rio_Types.h>

#if RIO_IS_WIN && defined(_WIN32)

#include <filedevice/rio_NativeFileDevice.h>

#include <misc/win/rio_Windows.h>

#include <algorithm>

namespace {

static inline std::string GetCWD()
{
    std::string path = std::string(MAX_PATH, '\0');
    GetModuleFileNameA(nullptr, &path[0], path.length());
    std::replace(path.begin(), path.end(), '\\', '/');
    return path.substr(0, path.find_last_of('/'));
}

}

namespace rio {

NativeFileDevice::NativeFileDevice()
    : StdIOFileDevice("native", GetCWD())
{
}

NativeFileDevice::NativeFileDevice(const std::string& drive_name)
    : StdIOFileDevice(drive_name, GetCWD())
{
}

} // namespace rio

#elif RIO_IS_WIN // Non-Windows Platforms

#include <filedevice/rio_NativeFileDevice.h>
#include <unistd.h>
#include <limits.h>
#include <algorithm>

namespace {
static inline std::string GetCWD() {
    char buffer[PATH_MAX];
    char* result = getcwd(buffer, sizeof(buffer));
    if (result) {
        std::string path(result);
        std::replace(path.begin(), path.end(), '\\', '/');
        return path;
    }
    return ""; // Return empty string if getcwd fails
}
}

namespace rio {

NativeFileDevice::NativeFileDevice()
    : StdIOFileDevice("native", GetCWD())
{
}

NativeFileDevice::NativeFileDevice(const std::string& drive_name)
    : StdIOFileDevice(drive_name, GetCWD())
{
}

} // namespace rio

#endif // RIO_IS_WIN
