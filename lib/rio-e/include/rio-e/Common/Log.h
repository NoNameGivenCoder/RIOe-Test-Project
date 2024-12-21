#pragma once

#include "rio.h"
#include "math/rio_Math.h"
#include "math/rio_MathTypes.h"
#include "math/rio_Matrix.h"

namespace rioe
{
	namespace Log
	{
        static void LogMatrix(const rio::Matrix34f& matrix)
        {
            for (int i = 0; i < 3; ++i)
            {
                RIO_LOG("%f %f %f %f\n", matrix.m[i][0], matrix.m[i][1], matrix.m[i][2], matrix.m[i][3]);
            }
        };

        static void LogMatrix(const rio::Matrix44f& matrix)
        {
            for (int i = 0; i < 4; ++i)
            {
                RIO_LOG("%f %f %f %f\n", matrix.m[i][0], matrix.m[i][1], matrix.m[i][2], matrix.m[i][3]);
            }
        };

        static void LogVector(const rio::Vector3f& vector)
        {
            RIO_LOG("%f %f %f\n", vector.x, vector.y, vector.z);
        }

        static void LogVector(const rio::Vector2f& vector)
        {
            RIO_LOG("%f %f\n", vector.x, vector.y);
        }
	}
}