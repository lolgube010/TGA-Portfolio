#pragma once
#define USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include <corecrt_math_defines.h>

// just math stuff.

namespace CommonUtilities
{
	template <typename T>
	T Max(const T aFirst, const T aSecond)
	{
		if (aFirst > aSecond)
		{
			return aFirst;
		}
		return aSecond;
	}

	template <typename T>
	T Min(const T aFirst, const T aSecond)
	{
		if (aFirst > aSecond)
		{
			return aSecond;
		}
		return aFirst;
	}

	template <typename T>
	T Abs(T aVar)
	{
		if (aVar < 0)
		{
			return aVar *= (-1);
		}
		return aVar;
	}

	template <typename T>
	T Clamp(const T aValue, const T aMin, const T aMax)
	{
		assert(aMin <= aMax);
		if (aValue < aMin)
		{
			return aMin;
		}
		if (aValue > aMax)
		{
			return aMax;
		}
		return aValue;
	}

	template <typename T>
	T Lerp(const T aFirst, const T aSecond, float aPercentage)
	{
		return (aFirst + aPercentage * (aSecond - aFirst));
	}

	template <typename T>
	void Swap(T& aFirst, T& aSecond)
	{
		T t = aFirst;
		aFirst = aSecond;
		aSecond = t;
	}

	inline float DegreesToRad(const float aDegrees)
	{
		return aDegrees * (static_cast<float>(M_PI) / 180);
	}

	inline float RadToDegrees(const float aRad)
	{
		return aRad * 180.f / static_cast<float>(M_PI);
	}

	constexpr float DegreesToRadConstExpr(const float aDegrees)
	{
		return aDegrees * (static_cast<float>(M_PI) / 180);
	}

	constexpr float RadToDegreesConstExpr(const float aRad)
	{
		return aRad * 180.f / static_cast<float>(M_PI);
	}

	// returns a positive float.
	inline float fModPositiveFloat(float a, float N)
	// https://stackoverflow.com/questions/4003232/how-to-code-a-modulo-operator-in-c-c-obj-c-that-handles-negative-numbers
	{
		return a - N * floor(a / N);
	}
}
