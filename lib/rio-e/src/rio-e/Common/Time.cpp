#include "rio-e/Common/Time.h"

namespace rioe
{
	Time::DateTime Time::mDateTime;
	f32 Time::mElapsedTime;

	const Time::DateTime Time::GetDistance(const DateTime& a, const DateTime& b)
	{
		DateTime val;

		val.day = a.day - b.day;
		val.hours = a.hours - b.hours;
		val.minutes = a.minutes - b.minutes;
		val.month = a.month - b.month;
		val.seconds = a.seconds - b.seconds;
		val.year = a.year - b.year;

		return val;
	}
}