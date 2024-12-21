#pragma once

#include "rio.h"

namespace rioe
{
	class Time
	{
	public:
		struct DateTime
		{
			u32 day;
			u32 month;
			u32 year;

			u32 seconds;
			u32 minutes;
			u32 hours;
		};

		// Returns current DateTime
		inline static const DateTime* GetDateTime() { return &mDateTime; };

		// Gets the distance between two DateTime
		inline static const DateTime GetDistance(const DateTime& a, const DateTime& b);

		inline static const f32 GetTime() { return mElapsedTime; };

		static void Calc();

		// TODO: get delta time from Engine class itself.
		//static f32 GetDeltaTime();
	private:
		static DateTime mDateTime;
		static f32 mElapsedTime;
	};
}

#if RIO_IS_WIN
#include "rio-e/Common/win/TimeImplWin.h"
#else
#include "rio-e/Common/cafe/TimeImplCafe.h"
#endif