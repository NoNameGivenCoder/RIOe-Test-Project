#include <math/rio_Quat.h>

namespace rioe
{
	namespace Utils
	{
		static rio::Quatf QuaternionFromEulerAngles(f32 roll, f32 pitch, f32 yaw)
		{
			rio::Quatf q;
			f32 cr, cp, cy, sr, sp, sy, cpcy, spsy;

			cr = cos(roll / 2);
			cp = cos(pitch / 2);
			cy = cos(yaw / 2);
			sr = sin(roll / 2);
			sp = sin(pitch / 2);
			sy = sin(yaw / 2);

			cpcy = cp * cy;
			spsy = sp * sy;

			q.w = cr * cpcy + sr * spsy;
			q.x = sr * cpcy - cr * spsy;
			q.y = cr * sp * cy + sr * cp * sy;
			q.z = cr * cp * sy - sr * sp * cy;

			return q;
		}

		static rio::Quatf QuaternionFromEulerAngles(const rio::Vector3f& eulerAngles)
		{
			return QuaternionFromEulerAngles(eulerAngles.x, eulerAngles.y, eulerAngles.z);
		}
	}
}