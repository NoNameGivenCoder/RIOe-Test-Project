#pragma once

#include <ctime>
#include <GLFW/glfw3.h>

#include "rio-e/Common/Time.h"

namespace rioe
{
	inline void Time::Calc()
	{
		mElapsedTime = glfwGetTime();

		time_t timestamp = time(0);
		tm* ltm = localtime(&timestamp);

		mDateTime.day = ltm->tm_mday;
		mDateTime.month = ltm->tm_mon + 1;
		mDateTime.year = 1900 + ltm->tm_year;

		mDateTime.seconds = ltm->tm_sec % 60;
		mDateTime.minutes = ltm->tm_min;
		mDateTime.hours = ltm->tm_hour;
	}
}