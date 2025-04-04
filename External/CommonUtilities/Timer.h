#pragma once
#include <chrono>
#include <functional>
#include <mutex>

namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer();

		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;

		void Update();
		void Reset();
		static void ExecuteAfter(const std::function<void()>& aFunc, double aDurationInSeconds);
		static void ExecuteEvery(const std::function<void()>& aFunc, double aIntervalInSeconds);

		// this function ignores if we've been paused or not.
		[[nodiscard]] double AbsoluteElapsedTime() const; // Returns elapsed time in seconds

		//This represents the total duration that the timer has been running since its creation or since it was last reset. It keeps increasing as time passes, regardless of whether the timer is paused or not. This is useful for tracking the overall duration of an operation or for time-based calculations.
		[[nodiscard]] double GetTotalSeconds() const;
		[[nodiscard]] float GetTotalSecondsFloat() const;

		[[nodiscard]] float GetDeltaTime() const;

		[[nodiscard]] double GetUnscaledTotalSeconds() const;
		[[nodiscard]] float GetUnscaledTotalSecondsFloat() const;
		[[nodiscard]] float GetUnscaledDeltaTime() const;

		[[nodiscard]] double GetFixedTotalSeconds() const;
		[[nodiscard]] float GetFixedTotalSecondsFloat() const;
		static float GetFixedDeltaTime();

		[[nodiscard]] bool GetIsPaused() const;
		[[nodiscard]] float GetTimeScale() const;

		void Pause(bool aIsPaused);
		void SetTimeScale(float aTimeScale);

		void Start();
	private:
		// same as typeDef
		using HighResClock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<HighResClock>;
		using Duration = std::chrono::duration<double>;

		double myTotalSeconds;
		float myDeltaTime;
		TimePoint myLastFrameTime;

		double myUnscaledTotalSeconds;
		float myUnscaledDeltaTime;

		double myFixedTotalSeconds;
		static constexpr float FIXED_DELTA_TIME = 0.016f;

		bool myIsPaused;
		float myTimeScale;

		std::mutex myEventMutex;
		std::condition_variable myEventCV;

		// For elapsed time
		TimePoint myAbsoluteStartTime;
	};

	inline Timer::Timer() : myTotalSeconds(0), myDeltaTime(0.0f), myLastFrameTime(HighResClock::now()),
	                        myUnscaledTotalSeconds(0),
	                        myUnscaledDeltaTime(0),
	                        myFixedTotalSeconds(0),
	                        myIsPaused(false), myTimeScale(1.f),
	                        myAbsoluteStartTime(HighResClock::now())
	{
		// empty
	}


	inline void Timer::Update()
	{
		const TimePoint currentTime = HighResClock::now();

		if (myIsPaused)
		{
			myLastFrameTime = currentTime;
			myDeltaTime = 0.0f;
			myUnscaledDeltaTime = 0.0f;
			return;
		}

		const Duration durationSinceLastFrame = currentTime - myLastFrameTime;

		// Calculate unscaled delta time and convert to seconds
		myUnscaledDeltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(durationSinceLastFrame).count();

		// Scale the delta time by timeScale
		myDeltaTime = myUnscaledDeltaTime * myTimeScale;

		// Update unscaled and scaled total time
		myUnscaledTotalSeconds += myUnscaledDeltaTime;
		myTotalSeconds += myDeltaTime;

		// Update fixed time and fixed delta time
		myFixedTotalSeconds += FIXED_DELTA_TIME;

		myLastFrameTime = currentTime;
	}

	inline void Timer::Reset()
	{
		std::lock_guard lock(myEventMutex);
		myTotalSeconds = 0.0;
		myUnscaledTotalSeconds = 0.0;
		myFixedTotalSeconds = 0.0;
		myIsPaused = false;
		// You may reset any other state variables here as needed
	}

	inline void Timer::ExecuteAfter(const std::function<void()>& aFunc, const double aDurationInSeconds)
	{
		std::thread([=]()
		{
			std::this_thread::sleep_for(std::chrono::duration<double>(aDurationInSeconds));
			aFunc();
		}).detach();
	}

	inline void Timer::ExecuteEvery(const std::function<void()>& aFunc, const double aIntervalInSeconds)
	{
		std::thread([=]()
		{
			while (true)
			{
				std::this_thread::sleep_for(std::chrono::duration<double>(aIntervalInSeconds));
				aFunc();
			}
		}).detach();
	}

	inline double Timer::AbsoluteElapsedTime() const
	{
		const TimePoint currentTime = HighResClock::now();
		const Duration durationSinceStart = currentTime - myAbsoluteStartTime;
		return durationSinceStart.count();
	}

	inline float Timer::GetDeltaTime() const
	{
		return myDeltaTime;
	}

	inline double Timer::GetUnscaledTotalSeconds() const
	{
		return myUnscaledTotalSeconds;
	}

	inline float Timer::GetUnscaledTotalSecondsFloat() const
	{
		return static_cast<float>(myUnscaledTotalSeconds);
	}

	inline float Timer::GetUnscaledDeltaTime() const
	{
		return myUnscaledDeltaTime;
	}

	inline double Timer::GetFixedTotalSeconds() const
	{
		return myFixedTotalSeconds;
	}

	inline float Timer::GetFixedTotalSecondsFloat() const
	{
		return static_cast<float>(myFixedTotalSeconds);
	}

	inline float Timer::GetFixedDeltaTime()
	{
		return FIXED_DELTA_TIME;
	}

	inline double Timer::GetTotalSeconds() const
	{
		return myTotalSeconds;
	}

	inline float Timer::GetTotalSecondsFloat() const
	{
		return static_cast<float>(myTotalSeconds);
	}

	inline bool Timer::GetIsPaused() const
	{
		return myIsPaused;
	}

	inline float Timer::GetTimeScale() const
	{
		return myTimeScale;
	}

	inline void Timer::Pause(const bool aIsPaused)
	{
		myIsPaused = aIsPaused;
	}

	inline void Timer::SetTimeScale(const float aTimeScale)
	{
		myTimeScale = aTimeScale;
	}
}
