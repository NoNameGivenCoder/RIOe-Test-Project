#include <coreinit/time.h>


#include "rio-e/Common/Time.h"

namespace rioe
{
	inline void Time::Calc()
	{
		// TODO: figure out how to use OS time func for this.
		mElapsedTime += 0.01f;

		OSTime timestamp = OSGetTime();
		OSCalendarTime ltm;
		OSTicksToCalendarTime(timestamp, &ltm);

		mDateTime.day = ltm.tm_mday;
		mDateTime.month = ltm.tm_mon + 1;
		mDateTime.year = ltm.tm_year;

		mDateTime.seconds = ltm.tm_sec % 60;
		mDateTime.minutes = ltm.tm_min;
		mDateTime.hours = ltm.tm_hour;
	}
}